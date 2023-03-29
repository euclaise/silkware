#include <stdint.h>
#include <paging.h>
#include <mem.h>
#include <assert.h>
#include <acpi.h>

struct
{
    acpi_header_t h;
    uint8_t p[];
} __attribute__((packed)) *xsdt;

void *acpi_find(const char *sig, size_t index)
{
    size_t count = -1;

    if (memcmp(sig, "DSDT", 4) == 0)
    {
        acpi_fadt_t *facp = acpi_find("FACP", 0);
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
