#include <u.h>
#include <assert.h>
#include <stdint.h>
#include <paging.h>
#include <io.h>
#include <memmap.h>
#include <mem.h>
#include <panic.h>
#include <miniheap.h>
#include <stdbool.h>

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
    pools = miniheap_alloc(memmap_len*sizeof(pool));
    for (i = 0; i < memmap_len; ++i)
    {
        size_t sz = prev_pow2(memmap[i].len);
        if (sz < PAGE_SIZE) continue;
        
        pools[n_pools].start = NULL;
        pools[n_pools].base = memmap[i].base;
        pools[n_pools].end = (block *) sz;

        ++n_pools;
    }
}

void *page_alloc(size_t sz)
{
    size_t i;
    sz = next_pow2(sz);
    if (sz < PAGE_SIZE) sz = PAGE_SIZE;

    for (i = 0; i < n_pools; ++i)
    {
        block *b;
        block *prev = NULL;

        if (pools[i].start == NULL)
        {
            void *phys = (void *) pools[i].base;
            size_t bsz = (size_t) pools[i].end;
            void *virt = kmap_phys(phys, bsz);


            if (bsz < sz) continue;
            pools[i].base = (uintptr_t) virt;
            pools[i].end = virt + bsz;
            pools[i].start = virt;
            pools[i].start->size = bsz;
            pools[i].start->next = pools[i].end;
        }

        for (b = pools[i].start; b < pools[i].end; b = b->next)
        {
            if (b->size >= sz)
            {
                block *res = split(b, sz, pools[i].base);
                if (prev) prev->next = res->next;
                else pools[i].start = res->next;
                return res;
            }
            prev = b;
        }
    }

    for (i = 0; i < n_pools; ++i)
        printf("Pool size: %lld\n", pools[i].end - pools[i].start);
    panic("page_alloc(%llu): Out of memory\n", sz);
    return NULL;
}

void page_free(void *pagev, size_t sz)
{
    size_t i;
    block *page = pagev;

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
                return;
            }

            if (b > page)
            {
                if (prev == NULL)
                {
                    page->next = pools[i].start;
                    pools[i].start = page;
                    return;
                }
                prev->next = page;
                page->next = b;
                return;
            }

            if (b == pools[i].end)
            {
                prev->next = page;
                page->next = pools[i].end;
                return;
            }
        }
    }

    /* Page not found in any pools */
    unreachable;
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
    size_t i;
    size_t i;
