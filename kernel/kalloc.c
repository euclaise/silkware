#include <u.h>
#include <types.h>
#include <page_alloc.h>
#include <rand.h>
#include <assert.h>
#include <panic.h>
#include <mem.h>
#include <mp.h>
#include <flex.h>

#define NBINS (128)
#define OVERALLOC_MAX (256)


/* NOTE: Size fields do NOT include the header size */
typedef struct block
{
    uintptr_t size;
    union
    {
        struct block *next;
        uint64_t canary;
    };
    int8_t data[];
} __attribute__((packed)) block;

#define HDRSIZE (sizeof(block))

struct kalloc_state /* Per-CPU state */
{
    block *start; /* Freed list: a linked list, ordered by address*/
};

static struct kalloc_state *states;

static inline uint64_t canary(uint64_t x)
{
	return srand_insecure(x);
}

void kalloc_init(void)
{
    states = page_zalloc(sizeof(struct kalloc_state)*ncpus);
}

static void insert_block(struct kalloc_state *state, block *new)
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

    if (new == cur) panic("Double free detected!");

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
    struct kalloc_state *state = &states[get_cpu_data()->id];

    /* Find fitting block and split it
     * Note: Blocks are ordered lowest-address first */
    prev = NULL;
    for (cur = state->start; cur != NULL; prev = cur, cur = cur->next)
    {
        /* Split block */
        if (cur->size >= size + HDRSIZE)
            /* Needs a header on the new free block, hence the + HDRSIZE */
        {
            new = (block *) (cur->data + size);
            new->size = cur->size - size - HDRSIZE;
            new->next = cur->next;
            if (prev) prev->next = new;
            else state->start = new;

            cur->size = size;
            cur->canary = canary((uint64_t) cur);

            assert_eq(cur, (uintptr_t) cur->data - HDRSIZE);
            return cur->data;
        }
    }

    /* No block found */
    res = page_alloc(size + HDRSIZE*2);
    page_blk_size = next_pow2(size + HDRSIZE*2);

    res->size = size;
    res->canary = canary((uint64_t) res);

    new = (block *) (res->data + size);
    new->size = page_blk_size - size - HDRSIZE*2;

    insert_block(state, new);

    assert_eq(res, (uintptr_t) res->data - HDRSIZE);
    return res->data;
}

void kfree(void *addr)
{
    block *b = (block *) ((uintptr_t) addr - HDRSIZE);
    /* TODO: Check that address is paged in */
    if (b->canary != canary((uint64_t) b)) panic("Heap overflow detected!");
    insert_block(&states[get_cpu_data()->id], b);
}

void *krealloc(void *old, size_t newsize)
{
    block *oldb = (block *) ((uintptr_t) old - HDRSIZE);
    void *res = kalloc(newsize);
    if (old)
    {
        memcpy(res, old, oldb->size);
        kfree(old);
    }
    return res;
}

void *kzalloc(size_t sz)
{
    void *res = kalloc(sz);
    if (res) memset(res, 0, sz);
    return res;
}
