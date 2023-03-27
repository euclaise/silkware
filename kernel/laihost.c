#include <stdint.h>
#include <lai/host.h>
#include <phys_malloc.h>
#include <paging.h>
#include <io.h>

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

void *laihost_map(size_t address, size_t count)
{
    return kmap_phys((void *) address, count);
}

void laihost_unmap(void *address, size_t count)
{
    kunmap(address, count);
}

void laihost_log(int level, const char *msg)
{
    (void)level;
    printf("%s\n", msg);
}
