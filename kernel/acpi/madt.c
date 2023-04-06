#include <acpi.h>
#include <panic.h>
#include <madt.h>

struct madt *madt;
uintptr_t lapic_phys;

void madt_init(void)
{
    int8_t *p;
    madt = acpi_find("APIC", 0);
    if (madt == NULL) panic("Could not find APIC");
    printf("MADT at %p\n", madt);
    lapic_phys = madt->lapic_addr;

    for (p = madt->data; p < (int8_t *) madt + madt->h.length; p += *(p + 1))
    {
        switch (*p)
        {
            case 5:
                lapic_phys = *((uint64_t *) (p + 4));
                break;
        }
    }
    printf("LAPIC at phys=%p\n", lapic_phys);
}
