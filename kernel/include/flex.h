#ifndef FLEX_H
#define FLEX_H

#include <phys_malloc.h>

#define FLEX(T) struct { size_t len; T data[]; } *
#define FLEX_ALLOC(T, size) phys_malloc(sizeof(size_t) + sizeof(T)*size)
#define FLEX_REALLOC(x, size) \
    phys_realloc((x), sizeof(size_t) + sizeof((x).data[0])*size)
#endif
