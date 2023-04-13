#include <types.h>
#include <kalloc.h>
#include <util.h>

#define Flex(T) struct { size_t n; T item[]; } _packed
#define FlexAlloc(T, n) kzalloc(sizeof(size_t) + sizeof(T)*(n))
#define FlexRealloc(x, n) \
    krealloc((x), sizeof(size_t) + sizeof((x)->item[0])*(n))
