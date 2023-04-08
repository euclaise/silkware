#include <acpi.h>
#include <panic.h>
#include <arch/apic.h>
#include <paging.h>

struct madt *madt;
static uintptr_t lapic_base;
uint8_t lapic_ids[512];

#define LAPIC_PHYS_ADDR (0)

#define LAPIC_CPUID (0)
#define LAPIC_ID (1)
#define LAPIC_FLAGS (2)

#define MADT_LAPIC (0)
#define MADT_64BIT (5)

void apic_init(void)
{
    uint16_t numcores = 0;
    struct record *p;
    if ((madt = acpi_find("APIC", 0)) == NULL)
        panic("Could not find APIC");
    printf("MADT at %p\n", madt);

    lapic_base = madt->lapic_addr;
    p = madt->data;
    for (
            p = madt->data;
            (void *)p < (void *)madt + madt->h.length;
            p = (void *)p + p->len
        )
    {
        switch (p->type)
        {
            case MADT_LAPIC:
                if (p->data[LAPIC_FLAGS] & 1 || p->data[LAPIC_FLAGS] & 2)
                    lapic_ids[numcores++] = p->data[LAPIC_ID];
                break;
            case MADT_64BIT: /* 64-bit LAPIC address override */
                lapic_base = *((uint64_t *) &p->data[LAPIC_PHYS_ADDR]);
                break;
        }
    }
    lapic_base = (uintptr_t) kmap_phys(lapic_base, PAGE_SIZE);
    printf("LAPIC at phys=%p\n", lapic_base);
}
