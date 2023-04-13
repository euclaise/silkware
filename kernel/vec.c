#include <vec.h>
#include <kalloc.h>
#include <types.h>
#include <mem.h>

struct vec_g /* generic */
{
    size_t c; /* Capacity */
    size_t n;
    byte item[];
};

void vec_push_fn(void **vv, void *x, size_t itemsz)
{
    struct vec_g *v = *vv;
    if (v == NULL)
    {
        v = kzalloc(sizeof(struct vec_g) + 8 * itemsz);
        v->c = 8 * itemsz;
        memcpy(&v->item[0], x, itemsz);
    }
    else
    {
        if (v->c == v->n) v = krealloc(v, sizeof(struct vec_g) + 1.5 * v->c);
        memcpy(&v->item[v->n], x, itemsz);
        v->n += itemsz;
    }
    *vv = v;
}

void vec_pop_fn(void *vv, size_t itemsz)
{
    ((struct vec_g *) vv)->n -= itemsz;
}
