#include <stdint.h>
#include <lai/host.h>
#include <phys_malloc.h>

void *laihost_malloc(size_t size)
{
    return phys_malloc(size);
}

void laihost_free(void *ptr, size_t size)
{
    (void)size;
    phys_free(ptr);
}

void *laihost_realloc(void *ptr, size_t newsize, size_t oldsize)
{
    (void)oldsize;
    return phys_realloc(ptr, newsize);
}
