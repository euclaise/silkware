#ifndef VEC_H
#define VEC_H

#include <types.h>

#define Vec(T) struct { size_t c; size_t n; T item[]; }

void vec_push_fn(void **vv, void *x, size_t itemsz);
void vec_pop_fn(void *vv, size_t itemsz);

#define vec_push(v, x) \
    (vec_push_fn((void **)(v), (void *)&(x), sizeof((*v)->item[0])))

#define vec_pop(v) (vec_pop_fn((void *) (v), sizeof((v)->item[0])))

#endif
