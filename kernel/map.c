#include <map.h>
#include <phys_malloc.h>
#include <util.h>
#include <stdbool.h>
#include <mem.h>

/* Hash function */
static uint64_t hash(const uint8_t *c /* Hashee */, int n /* Size */)
{
    int i;
    uint64_t h = 0xcbf29ce484222325; /* FNV offset basis */
    for (i = 0; i < n; ++i)
    {
        h ^= c[i];
        h *= 0x100000001b3; /* FNV prime */
    }
    return h;
}

void map_set(
        map *m,
        const uint8_t *k /* Key */,
        int kn, /* Key length */
        const uint8_t *v /* Data */,
        int vn /* Data length */
    )
{
    uint64_t h = hash(k, kn);
    bucket *b = &m->item[h % m->n];
    size_t i;

    for (i = 0; i < b->n; ++i)
        if (b->item[i].h == h && likely(memcmp(b->item[i].k, k, kn) == 0))
            {
                b->item[i].v = FLEX_REALLOC(b->item[i].v, vn);
                b->item[i].v->n = vn;
                memcpy(&b->item[i].v->item, v, vn);
                return;
            }

    if (b->cap == b->n)
    {
        b = phys_realloc(b, sizeof(bucket) + sizeof(map_item)*(b->cap * 1.5));
        b->cap *= 1.5;
    }
    map_item *item = &b->item[b->n];
    item->h = h;
    item->k = FLEX_ALLOC(int8_t, kn);
    item->v = FLEX_ALLOC(int8_t, vn);
    memcpy(item->k->item, k, kn);
    memcpy(item->k->item, v, vn);
}

void map_free(map *m)
{
    size_t i, j;
    for (i = 0; i < m->n; ++i) /* For each bucket */
        for (j = 0; j < m->item[i].n; ++i) /* For each item in bucket */
        {
            phys_free(m->item[i].item[j].k);
            phys_free(m->item[i].item[j].v);
        }
    phys_free(m);
}

uint8_t *map_get(map *m, const uint8_t *k, size_t n)
{
    uint64_t h = hash(k, n);
    bucket *b = &m->item[h % m->n];
    size_t i;

    for (i = 0; i < b->n; ++i)
        if (b->item[i].h == h && likely(memcmp(b->item[i].k, k, n) == 0))
            return b->item[i].v->item;

    return NULL;
}
