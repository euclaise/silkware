#include <map.h>
#include <io.h>
#include <util.h>
#include <types.h>
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
    if (n < 8) n = 8;
    map *res = kzalloc(sizeof(map) + sizeof(struct map_item)*n);
    res->cap = n;
    return res;
}

void *map_get(const map *m, const void *k, size_t kn)
{
    uint64_t h = hash(k, kn);
    size_t idx = h % m->cap;
    size_t i;

    if (m->max == 0) return NULL;

    if (idx > m->max) idx = m->max;
    i = idx;
    do
    {
        i = (i + 1) % (m->max + 1);
        if (m->cell[i].k
                && m->cell[i].h == h
                && likely(m->cell[i].k->n == kn)
                && likely(memcmp(m->cell[i].k->item, k, kn) == 0))
            return m->cell[i].v->item;
    } while (i != idx);
    return NULL;
}

map *map_grow(map *m, size_t n)
{
    size_t i;
    map *new;
    
    if (n <= m->cap) return m;
    new = map_new(n);

    for (i = 0; i <= m->max; ++i)
        if (m->cell[i].k)
        {
            size_t idx = m->cell[i].h % new->cap;
            while (new->cell[idx].k) idx = (idx + 1) % m->cap;
            new->cell[idx].h = m->cell[i].h;
            new->cell[idx].k = m->cell[i].k;
            new->cell[idx].v = m->cell[i].v;
        }

    new->cap = m->cap;
    kfree(m);
    return new;
}

void map_insert(map **mp, const void *k, size_t kn, const void *v, size_t vn)
{
    map *m = *mp;
    uint64_t h = hash(k, kn);
    size_t idx = h % m->cap;
    size_t i = idx;

    do { i = (i + 1) % m->cap; } while (m->cell[i].k && i != idx);

    if (m->cell[i].k == NULL)
    {
        m->cell[i].h = h;

        m->cell[i].k = FlexAlloc(int8_t, kn);
        m->cell[i].k->n = kn;
        memcpy(m->cell[i].k->item, k, kn);

        m->cell[i].v = FlexAlloc(int8_t, vn);
        m->cell[i].v->n = vn;
        memcpy(m->cell[i].v->item, v, vn);
        if (i > m->max) m->max = i;
        return;
    }

    m = map_grow(m, m->cap * 1.5);

    i = idx = h % m->cap;
    do { i = (i + 1) % m->cap; } while (m->cell[i].k && i != idx);
    if (i == idx) unreachable;

    m->cell[i].h = h;
    m->cell[i].k = FlexAlloc(int8_t, kn);
    m->cell[i].k->n = kn;
    memcpy(m->cell[i].k, k, kn);

    m->cell[i].v = FlexAlloc(int8_t, vn);
    m->cell[i].v->n = vn;
    memcpy(m->cell[i].v, v, vn);
    if (i > m->max) m->max = i;
    *mp = m;
}

void map_del(map *m, const void *k, size_t kn)
{
    uint64_t h = hash(k, kn);
    size_t idx = h % m->cap;
    size_t i;

    if (m->max == 0) return;

    if (idx > m->max) idx = m->max;
    i = idx;

    do
    {
        i = (i + 1) % (m->max + 1);
        if (m->cell[i].k
                && m->cell[i].h == h
                && likely(m->cell[i].k->n == kn)
                && likely(memcmp(m->cell[i].k->item, k, kn) == 0))
        {
            kfree(m->cell[i].k);
            kfree(m->cell[i].v);
            m->cell[i].k = NULL;
            return;
        }
    } while (i != idx);
}

void map_set(map **mp, const void *k, size_t kn, const void *v, size_t vn)
{
    map_del(*mp, k, kn);
    map_insert(mp, k, kn, v, vn);
}

void map_free(map *m)
{
    size_t i;

    for (i = 0; i <= m->max; ++i)
        if (m->cell[i].k)
        {
            kfree(m->cell[i].k);
            kfree(m->cell[i].v);
        }
    kfree(m);
}
