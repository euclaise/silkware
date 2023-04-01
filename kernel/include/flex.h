#include <stddef.h>
#include <kalloc.h>
#include <util.h>

#define FLEX(T) struct { size_t n; T item[1]; } _packed
#define FLEX_ALLOC(T, n) kalloc(sizeof(size_t) + sizeof(T)*(n))
#define FLEX_REALLOC(x, n) \
    krealloc((x), sizeof(size_t) + sizeof((x)->item[0])*(n))
