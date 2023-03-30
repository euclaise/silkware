#include <stdint.h>
#include <paging.h>
#include <io.h>
#include <memmap.h>
#include <mem.h>
#include <panic.h>

#define N_POOLS 128

typedef struct block
{
    uintptr_t size;
    struct block *next;
} block;

typedef struct pool
{
    block *start;
    uintptr_t base;
    block *end;
} pool;

static pool pools[N_POOLS];

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
    int64_t j = 0;
    for (i = 0; i < memmap_len; ++i)
    {
        size_t sz = prev_pow2(memmap[i].len);
        if (sz < 0x1000) continue;
        
        pools[j].base = (uintptr_t) kmap_phys((void *) memmap[i].base, sz);
        pools[j].end = (block *) (pools[j].base + sz);
        pools[j].start = (block *) pools[j].base;
        pools[j].start->size = sz;
        pools[j].start->next = pools[j].end;
        ++j;
    }
}

void *page_alloc(size_t sz)
{
    sz = next_pow2(sz);
    if (sz < 0x1000) sz = 0x1000;
    size_t i;
    for (i = 0; i < N_POOLS; ++i)
    {
        block *b;
        block *prev = NULL;
        for (b = pools[i].start; b < pools[i].end; prev = b, b = b->next)
            if (b->size >= sz)
            {
                block *res = split(b, sz, pools[i].base);
                if (prev) prev->next = res->next;
                else pools[i].start = res->next;
                return res;
            }
    }

    panic("WARNING: Out of memory!");
    return NULL;
}

void page_free(void *pagev, size_t sz)
{
    size_t i;
    block *page = pagev;

    sz = next_pow2(sz);
    if (sz < 0x1000) sz = 0x1000;
    page->size = sz;

    for (i = 0; i < N_POOLS; ++i)
    {
        block *b;
        block *prev = NULL;
        if (page < pools[i].start || page > pools[i].end) continue;

        for (b = pools[i].start; b < pools[i].end; b = b->next)
        {
            block *bb = (block *) get_buddy((uintptr_t) page, pools[i].base);
            if (b == (block *) bb)
            {
                block *h, *l; /* High, low */
                if (b > bb)
                {
                    h = b;
                    l = bb;
                }
                else
                {
                    h = bb;
                    l = b;
                }

                l->size += h->size;
                l->next = b->next;
                prev->next = l;
                return;
            }
            if (page < b && prev == NULL)
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
        }
    }
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
