#include <stdint.h>
#include <lai/host.h>
#include <phys_malloc.h>
#include <paging.h>
#include <io.h>
#include "acpi.h"

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

static rsdt_t *rsdt;
void *laihost_scan(const char *sig, size_t index)
{
    size_t count = -1;

    if (!rsdt) 
        rsdt = kmap_phys((void *) (uint64_t) rsdp.addr, sizeof(sdt_header));

    for (size_t i = 0; i < (rsdt->h.length - sizeof(rsdt->h)) / 4; ++i)
    {
        sdt_header *t = kmap_phys(
            (void *) (uint64_t) rsdt->p[i],
            sizeof(sdt_header)
        );

        if (*(uint32_t *)sig == *(uint32_t *)t->signature) count++;
        if (count == index) return t;

        if ((void *)t != (void *)rsdt) kunmap(t, sizeof(sdt_header));
    }
    return NULL;
}
