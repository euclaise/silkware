#include <stdint.h>
#include <paging.h>
#include <mem.h>
#include <assert.h>
#include <acpi.h>
#include <io.h>

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
        acpi_header_t *dsdt;
        uint32_t length;
        acpi_fadt_t *facp = acpi_find("FACP", 0);
        uintptr_t phys = (acpi64 ? facp->x_dsdt : (uint64_t) facp->dsdt);
        
        dsdt = map_anon(
            phys,
            sizeof(acpi_header_t),
            PAGE_PRESENT | PAGE_WRITABLE | PAGE_NX
        );
        length = dsdt->length;

        unmap_pages(dsdt, sizeof(acpi_header_t));
        return map_anon(phys, length, PAGE_PRESENT | PAGE_WRITABLE | PAGE_NX);
    }

    if (xsdt == NULL)
    {
        uintptr_t phys = (acpi64 ? xsdp.xsdt : (uint64_t) xsdp.rsdt);

        xsdt = map_anon(
            phys,
            sizeof(acpi_header_t),
            PAGE_PRESENT | PAGE_WRITABLE | PAGE_NX
        );
        uint32_t length = xsdt->h.length;

        unmap_pages(xsdt, sizeof(acpi_header_t));
        xsdt = map_anon(phys, length, PAGE_PRESENT | PAGE_WRITABLE | PAGE_NX);

        assert(memcmp(xsdt->h.signature + 1, "SDT", 3) == 0);
    }

    size_t entries = (xsdt->h.length - sizeof(xsdt->h)) / (acpi64 ? 8 : 4);
    for (size_t i = 0; i < entries; ++i)
    {
        acpi_header_t *t;
        uintptr_t phys;
        uint32_t length;
        if (acpi64) phys = ((uint64_t *) xsdt->p)[i];
        else phys = (uintptr_t) ((uint32_t *) xsdt->p)[i];

        t = map_anon(
            phys,
            sizeof(acpi_header_t),
            PAGE_PRESENT | PAGE_WRITABLE | PAGE_NX
        );
        length = t->length;

        unmap_pages(t, sizeof(acpi_header_t));
        t = map_anon(phys, length, PAGE_PRESENT | PAGE_WRITABLE | PAGE_NX);
        
        if (memcmp(sig, t->signature, 4) == 0) ++count;
        if (count == index) return t;

        if (t != (acpi_header_t *)xsdt) unmap_pages(t, length);
    }
    return NULL;
}
