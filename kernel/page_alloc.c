#include <u.h>
#include <assert.h>
#include <types.h>
#include <paging.h>
#include <io.h>
#include <memmap.h>
#include <mem.h>
#include <panic.h>
#include <miniheap.h>
#include <lock.h>

typedef struct block
{
    uintptr_t size;
    struct block *next;
} block;

typedef struct pool
{
    block *start; /* NULL if unpaged */
    uintptr_t base;
    block *end;
} pool; /* Ordered by address */

static pool *pools;
size_t n_pools;
static lock_t lock = LOCK_INIT;

uintptr_t prev_pow2(uintptr_t x)
{
    size_t s;
    if (x == 0) return 0;

    for (s = 1; s < sizeof(s)*8; s <<= 1) x |= x >> s;
    return x - (x >> 1);
}

uintptr_t next_pow2(uintptr_t x)
{
    size_t s;
    if (x == 0) return 0;

    x--;
    for (s = 1; s < sizeof(s)*8; s <<= 1) x |= x >> s;
    return ++x;
}

uintptr_t get_buddy(uintptr_t addr, uintptr_t base)
{
    return base + ((addr - base) ^ ((block *)addr)->size);
}

block *split(block *b, size_t sz, uintptr_t base)
{
    assert(b->size >= sz);
    while (b->size != sz)
    {
        block *bb;
        b->size >>= 1;
        bb = (block *) get_buddy((uintptr_t) b, base);
        *bb = *b;
        b->next = bb;
    }
    return b;
}

void page_alloc_init(void)
{
    size_t i;
    size_t j = 0;

    for (i = 0; i < memmap_len; ++i)
    {
        size_t len = memmap[i].len;
        if (unlikely(len < PAGE_SIZE))
            continue; /* Probably never happens in reality */

        do {
            ++n_pools;
            len -= prev_pow2(len);
        } while (len - prev_pow2(len) >= PAGE_SIZE);
    }
    pools = miniheap_alloc(n_pools*sizeof(pool));

    for (i = 0; i < memmap_len; ++i)
    {
        uintptr_t phys = memmap[i].base;
        size_t sz = memmap[i].len;
        size_t szp2 = prev_pow2(sz);
        if (unlikely(sz < PAGE_SIZE)) continue;

        /* 
         * Buddy allocator requires power-of-2 sizes, so we get the largest pow2
         * size for each memmap block, then retry again with the rest of the
         * block if there's more left over
         */
        do {
            void *virt;
            virt = kmap_phys(phys, szp2);
            pools[j].base = (uintptr_t) virt;
            pools[j].end = (void *)((uintptr_t) virt + szp2);
            pools[j].start = virt;
            pools[j].start->size = szp2;
            pools[j].start->next = pools[j].end;

            sz -= szp2;
            phys += szp2;
            szp2 = prev_pow2(sz);

            ++j;
        } while (sz - szp2 >= PAGE_SIZE);
    }
}

void *page_alloc(size_t sz)
{
    size_t i;
    assert(pools != NULL);
    ACQUIRE(&lock);

    sz = next_pow2(sz);
    if (sz < PAGE_SIZE) sz = PAGE_SIZE;

    for (i = 0; i < n_pools; ++i)
    {
        block *b;
        block *prev = NULL;

        for (b = pools[i].start; b < pools[i].end; b = b->next)
        {
            if (b->size >= sz)
            {
                block *res = split(b, sz, pools[i].base);
                if (prev) prev->next = res->next;
                else pools[i].start = res->next;
                RELEASE(&lock);
                return res;
            }
            prev = b;
        }
    }

    for (i = 0; i < n_pools; ++i)
        printf(
            "Pool size: %lld (%p)\n",
            pools[i].end - pools[i].start,
            pools[i].base
        );
    panic("page_alloc(%llu): Out of memory\n", sz);
    RELEASE(&lock);
    return NULL;
}

void page_free(void *pagev, size_t sz)
{
    size_t i;
    block *page = pagev;

    ACQUIRE(&lock);

    sz = next_pow2(sz);
    if (sz < PAGE_SIZE) sz = PAGE_SIZE;
    page->size = sz;

    for (i = 0; i < n_pools; ++i)
    {
        block *b;
        block *prev = NULL;
        if (page < (block *) pools[i].base || page > pools[i].end) continue;

        for (b = pools[i].start; ; b = b->next)
        {
            if (b == (block *) get_buddy((uintptr_t) page, pools[i].base))
            { /* Found buddy in free-list */
                block *h, *l; /* High, low */
                if (b > page)
                {
                    h = b;
                    l = page;
                }
                else
                {
                    h = page;
                    l = b;
                }

                l->size += h->size;
                l->next = b->next;
                if (prev) prev->next = l;
                else pools[i].start = l;
                goto end;
            }

            if (b > page)
            {
                if (prev == NULL)
                {
                    page->next = pools[i].start;
                    pools[i].start = page;
                    goto end;
                }
                prev->next = page;
                page->next = b;
                goto end;
            }

            if (b == pools[i].end)
            {
                prev->next = page;
                page->next = pools[i].end;
                goto end;
            }
        }
    }

    /* Page not found in any pools */
    unreachable;

end:
    RELEASE(&lock);
}

void *page_realloc(void *page, size_t oldsize, size_t newsize)
{
    void *res = page_alloc(newsize);
    if (page)
    {
        memcpy(res, page, oldsize);
        page_free(page, oldsize);
    }
    return res;
}

void *page_zalloc(size_t size)
{
    void *res = page_alloc(size);
    if (res) memset(res, 0, size);
    return res;
}
