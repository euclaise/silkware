#ifndef FLEX_H
#define FLEX_H

#include <phys_malloc.h>

#define FLEX(T) struct { size_t n; T item[1]; }
#define FLEX_ALLOC(T, size) phys_malloc(sizeof(size_t) + sizeof(T)*((size) - 1))
#define FLEX_REALLOC(x, size) \
    phys_realloc((x), sizeof(size_t) + sizeof((x)->item[0])*((size) - 1))
#endif
