#include <stdint.h>
#include <lai/host.h>
#include <phys_malloc.h>
#include <paging.h>
#include <io.h>
#include <panic.h>
#include <mem.h>
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

void laihost_panic(const char *msg)
{
    panic("%s", msg);
    __builtin_unreachable();
}

static rsdt_t *rsdt;
void *laihost_scan(const char *sig, size_t index)
{
    size_t count = -1;

    if (!rsdt) 
        rsdt = kmap_phys((void *) (uint64_t) rsdp.rsdt, sizeof(acpi_header_t));

    size_t entries = (rsdt->h.length - sizeof(rsdt->h)) / 4;
    for (size_t i = 0; i < entries; ++i)
    {
        acpi_header_t *t = kmap_phys(
            (void *) (uint64_t) rsdt->p[i],
            sizeof(acpi_header_t)
        );
        
        if (memcmp(sig, t->signature, 4) == 0) count++;
        if (count == index) return t;

        if (t != (acpi_header_t *)rsdt) kunmap(t, sizeof(acpi_header_t));
    }
    return NULL;
}
