#include <acpi.h>
#include <panic.h>
#include <arch/apic.h>
#include <paging.h>
#include <mp.h>
#include <assert.h>
#include <timer.h>
#include <arch/x86.h>

struct madt *madt;
static uintptr_t lapic_base;
uint8_t lapic_ids[512];

enum
{
    LAPIC_PHYS_ADDR = 0,

    LAPIC_CPUID = 0,
    LAPIC_ID    = 1,
    LAPIC_FLAGS = 2,
    
    MADT_LAPIC = 0,
    MADT_64BIT = 5
};

uint32_t lapic_read(uint32_t reg)
{
    return *(volatile uint32_t *)(lapic_base + reg);
}

void lapic_write(uint32_t reg, uint32_t value)
{
    *(volatile uint32_t *)(lapic_base + reg) = value;
}

void lapic_eoi(void)
{
    lapic_write(LAPIC_REG_EOI, 0);
}

void apic_init(void)
{
    uint16_t numcores = 0;
    struct record *p;

    madt = acpi_find("APIC", 0);
    if (madt == NULL) panic("Could not find APIC");

    printf("MADT at %p\n", madt);

    lapic_base = madt->lapic_addr;
    for (
            p = madt->data;
            (uintptr_t)p < (uintptr_t)madt + madt->h.length;
            p = (struct record *)((uintptr_t)p + p->len)
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

    assert_eq(numcores, ncpus);
    printf("LAPIC at phys=%p\n", lapic_base);
    lapic_base = (uintptr_t) kmap_phys(lapic_base, PAGE_SIZE);
    apic_start();
}

static uint32_t ticks; /* Ticks in 1 ms */

void apic_set_duration(uint32_t us)
{
    uint32_t ticks_value = (ticks * us) / 1000;
    if (ticks_value == 0) ticks_value = 1;
    
    lapic_write(LAPIC_REG_LVT_TMR, INT_TIMER | LAPIC_TMR_PERIODIC);
    lapic_write(LAPIC_REG_TMR_DIV, APIC_TMR_DIV_BY_16);

    lapic_write(LAPIC_REG_TMR_INITIAL, ticks_value);

}

void apic_start(void)
{
    lapic_write(LAPIC_REG_SPURIOUS,
        lapic_read(LAPIC_REG_SPURIOUS)
        | LAPIC_SPURIOUS_ALL
        | LAPIC_SPURIOUS_ENABLE
    );

    lapic_write(LAPIC_REG_TMR_DIV, APIC_TMR_DIV_BY_16);
    lapic_write(LAPIC_REG_TMR_INITIAL, 0xFFFFFFFF); /* -1 */

    timer_sleep_ms(10);
    lapic_write(LAPIC_REG_LVT_TMR, LAPIC_TMR_MASKED);
    ticks = (0xFFFFFFFF - lapic_read(LAPIC_REG_TMR_CUR)) / 10;
    if (ticks == 0) ticks = 1;

    apic_set_duration(1000 * 10); /* 10 ms */
}
