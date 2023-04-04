#include <map.h>
#include <io.h>
#include <stdint.h>
#include <util.h>
#include <stdbool.h>
#include <mem.h>
#include <assert.h>

/* Simple cache-friendly hashmap inspired by swiss tables */

/* Hash function */
static uint64_t hash(const int8_t *c /* Hashee */, size_t n /* Size */)
{
    size_t i;
    uint64_t h = 0xcbf29ce484222325; /* FNV offset basis */
    for (i = 0; i < n; ++i)
    {
        h ^= c[i];
        h *= 0x100000001b3; /* FNV prime */
    }
    return h;
}

map *map_new(size_t n)
{
    size_t i;
    map *res = kalloc(sizeof(map) + sizeof(struct map_item)*(n-1));
    res->n = 0;
    res->cap = n;
    for (i = 0; i < res->cap; ++i)
    {
        res->cell[i].h = 0;
        res->cell[i].k = NULL;
        res->cell[i].v = NULL;
    }
    return res;
}

void *map_get(const map *m, const void *k, size_t kn)
{
    uint64_t h = hash(k, kn);
    size_t idx = h % m->cap;
    size_t i = idx;

    do
    {
        if (m->cell[i].k
                && m->cell[i].h == h
                && likely(m->cell[i].k->n == kn)
                && likely(memcmp(m->cell[i].k->item, k, kn) == 0))
            return m->cell[i].v->item;
        i = (i + 1) % m->cap;
    } while (i != idx);
    return NULL;
}

map *map_grow(map *m, size_t n)
{
    size_t i;
    map *new;
    
    if (n <= m->n) return NULL;
    new = map_new(n);

    for (i = 0; i < m->n; ++i)
        if (m->cell[i].k)
        {
            size_t idx = m->cell[i].h % new->cap;
            while (new->cell[idx].k) idx = (idx + 1) % m->cap;
            new->cell[idx].h = m->cell[i].h;
            new->cell[idx].k = m->cell[i].k;
            new->cell[idx].v = m->cell[i].v;
        }

    new->n = m->n;
    kfree(m);
    return new;
}

void map_set(map **mp, const void *k, size_t kn, const void *v, size_t vn)
{
    map *m = *mp;
    uint64_t h = hash(k, kn);
    size_t idx = h % m->cap;
    size_t i = idx;

    do { i = (i + 1) % m->cap; } while (m->cell[i].k && i != idx);

    if (m->cell[i].k == NULL)
    {
        m->cell[i].h = h;

        m->cell[i].k = FLEX_ALLOC(int8_t, kn);
        m->cell[i].k->n = kn;
        memcpy(m->cell[i].k->item, k, kn);

        m->cell[i].v = FLEX_ALLOC(int8_t, vn);
        m->cell[i].v->n = vn;
        memcpy(m->cell[i].v->item, v, vn);

        return;
    }

    m = map_grow(m, m->cap * 1.25);

    i = idx = h % m->cap;
    do { i = (i + 1) % m->cap; } while (m->cell[i].k && i != idx);
    if (i == idx) unreachable;

    m->cell[i].h = h;
    m->cell[i].k = FLEX_ALLOC(int8_t, kn);
    m->cell[i].k->n = kn;
    memcpy(m->cell[i].k, k, kn);

    m->cell[i].v = FLEX_ALLOC(int8_t, vn);
    m->cell[i].v->n = vn;
    memcpy(m->cell[i].v, v, vn);
    *mp = m;
}

void map_free(map *m)
{
    size_t i;
    for (i = 0; i < m->cap; ++i)
        if (m->cell[i].k)
        {
            kfree(m->cell[i].k);
            kfree(m->cell[i].v);
        }
    kfree(m);
}
