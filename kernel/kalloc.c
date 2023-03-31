#include <u.h>
#include <page_alloc.h>
#include <rand.h>
#include <stdint.h>
#include <assert.h>
#include <panic.h>
#include <mem.h>

#define NBINS (128)
#define OVERALLOC_MAX (256)

static uint64_t canary_seed;


/* NOTE: Size fields do NOT include the header size */
typedef struct block
{
    uintptr_t size;
    union
    {
        struct block *next;
        uint64_t canary;
    };
    int8_t data[1];
} __attribute__((packed)) block;

#define HDRSIZE (sizeof(block) - sizeof(int8_t[1]))

typedef struct kalloc_state /* Per-CPU state */
{
    block *start; /* Freed list: a linked list, ordered by address*/
} kalloc_state;

kalloc_state state;

void kalloc_init(void)
{
    canary_seed = rand64();
}

static void insert_block(kalloc_state *state, block *new)
{
    block *cur, *prev;
    prev = state->start;

    if (prev == NULL)
    {
        new->next = prev;
        state->start = new;
        return;
    }

    cur = prev->next;

    while (new < cur)
    {
        prev = cur;
        cur = cur->next;
    }

    prev->next = new;
    new->next = cur;

    if (prev->data + prev->size == (void *) new)
    {
        prev->next = new->next;
        prev->size += new->size + HDRSIZE;
        new = prev;
    }

    if (new->data + new->size == (void *) cur)
    {
        new->next = cur->next;
        new->size += cur->size + HDRSIZE;
    }
}

void *kalloc(size_t size)
{
    block *cur, *prev, *res, *new;
    size_t page_blk_size;
    size = (size + (ALLOC_ALIGN - 1)) & ~(ALLOC_ALIGN - 1); /* Align up */

    /* Find fitting block and split it
     * Note: Blocks are ordered lowest-address first */
    prev = NULL;
    for (cur = state.start; cur != NULL; prev = cur, cur = cur->next)
    {
        /* Split block */
        if (cur->size >= size + HDRSIZE)
            /* Needs a header on the new free block, hence the + HDRSIZE */
        {
            new = (block *) (cur->data + size);
            new->size = cur->size - size - HDRSIZE;
            new->next = cur->next;
            prev->next = new;

            cur->size = size;
            cur->canary = (uint64_t) cur ^ canary_seed;

            assert((void *) cur == (void *) cur->data - HDRSIZE);
            return cur->data;
        }
    }

    /* No block found */
    res = page_alloc(size + HDRSIZE*2);
    page_blk_size = next_pow2(size + HDRSIZE*2);

    res->size = size;
    res->canary = (uint64_t) res ^ canary_seed;

    new = (block *) (res->data + size);
    new->size = page_blk_size - size - HDRSIZE*2;

    insert_block(&state, new);

    assert((void *) res == (void *) res->data - HDRSIZE);
    return res->data;
}

void kfree(void *addr)
{
    block *b = addr - HDRSIZE;
    /* TODO: Check that address is paged in */
    if (b->canary != ((uint64_t) b ^ canary_seed))
        panic("Heap overflow detected!");
    insert_block(&state, b);
}

void *krealloc(void *old, size_t newsize)
{
    block *oldb = old - HDRSIZE;
    void *res = kalloc(newsize);
    if (old)
    {
        memcpy(res, old, oldb->size);
        kfree(old);
    }
    return res;
}
