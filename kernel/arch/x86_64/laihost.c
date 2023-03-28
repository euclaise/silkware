#include <stdint.h>
#include <lai/host.h>
#include <phys_malloc.h>
#include <paging.h>
#include <io.h>
#include <panic.h>
#include <mem.h>
#include <assert.h>
#include "ports.h"
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
    void *res = phys_realloc(ptr, newsize);
    if (res < (void *) 0x1000) panic("Bad malloc\n");
    return res;
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
        void *phys = (void *) (acpi64 ? facp->x_dsdt : (uint64_t) facp->dsdt);
        
        acpi_header_t *dsdt = kmap_phys(phys, sizeof(acpi_header_t));
        uint32_t length = dsdt->length;

        kunmap(dsdt, sizeof(acpi_header_t));
        return kmap_phys(phys, length);
    }

    if (!xsdt) 
    {
        void *phys = (void *) (acpi64 ? xsdp.xsdt : (uint64_t) xsdp.rsdt);

        xsdt = kmap_phys(phys, sizeof(acpi_header_t));
        uint32_t length = xsdt->h.length;

        kunmap(xsdt, sizeof(acpi_header_t));
        xsdt = kmap_phys(phys, length);

        assert(memcmp(xsdt->h.signature + 1, "SDT", 3) == 0);
    }

    size_t entries = (xsdt->h.length - sizeof(xsdt->h)) / (acpi64 ? 8 : 4);
    for (size_t i = 0; i < entries; ++i)
    {
        uintptr_t phys;
        if (acpi64) phys = ((uint64_t *) xsdt->p)[i];
        else phys = (uintptr_t) ((uint32_t *) xsdt->p)[i];

        acpi_header_t *t = kmap_phys((void *) phys, sizeof(acpi_header_t));
        uint32_t length = t->length;

        kunmap(t, sizeof(acpi_header_t));
        t = kmap_phys((void *) phys, length);
        
        if (memcmp(sig, t->signature, 4) == 0) count++;
        if (count == index) return t;

        if (t != (acpi_header_t *)xsdt) kunmap(t, length);
    }
    return NULL;
}

uint8_t laihost_inb(uint16_t port)
{
    return port_inb(port);
}

uint16_t laihost_inw(uint16_t port)
{
    return port_inw(port);
}

uint32_t laihost_ind(uint16_t port)
{
    return port_ind(port);
}

void laihost_outb(uint16_t port, uint8_t x)
{
    port_outb(port, x);
}

void laihost_outw(uint16_t port, uint16_t x)
{
    port_outb(port, x);
}

void laihost_outd(uint16_t port, uint32_t x)
{
    port_outd(port, x);
}
