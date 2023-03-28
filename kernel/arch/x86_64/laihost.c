#include <stdint.h>
#include <lai/host.h>
#include <phys_malloc.h>
#include <paging.h>
#include <io.h>
#include <panic.h>
#include <mem.h>
#include <assert.h>
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

struct
{
    acpi_header_t h;
    uint8_t p[];
} __attribute__((packed)) *xsdt;

void *laihost_scan(const char *sig, size_t index)
{
    size_t count = -1;

    if (memcmp(sig, "DSDT", 4) == 0)
    {
        acpi_fadt_t *facp = laihost_scan("FACP", 0);
        
        return kmap_phys(
            (void *) (acpi64 ? facp->x_dsdt : (uint64_t) facp->dsdt),
            sizeof(acpi_xsdt_t)
        );
    }

    if (!xsdt) 
    {
        xsdt = kmap_phys(
            (void *) (acpi64 ? xsdp.xsdt : (uint64_t) xsdp.rsdt),
            sizeof(acpi_header_t)
        );
        assert(memcmp(xsdt->h.signature + 1, "SDT", 3) == 0);
    }

    size_t entries = (xsdt->h.length - sizeof(xsdt->h)) / (acpi64 ? 8 : 4);
    for (size_t i = 0; i < entries; ++i)
    {
        uintptr_t phys;
        if (acpi64) phys = ((uint64_t *) xsdt->p)[i];
        else phys = (uintptr_t) ((uint32_t *) xsdt->p)[i];

        acpi_header_t *t = kmap_phys((void *) phys, sizeof(acpi_header_t));
        
        if (memcmp(sig, t->signature, 4) == 0) count++;
        if (count == index) return t;

        if (t != (acpi_header_t *)xsdt) kunmap(t, sizeof(acpi_header_t));
    }
    return NULL;
}
