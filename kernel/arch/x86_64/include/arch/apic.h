#ifndef MADT_H
#define MADT_H

#include <stdint.h>
#include <util.h>
#include <acpispec/tables.h>

struct record
{
    int8_t type;
    int8_t len;
    int8_t data[1];
} _packed;

struct madt
{
    acpi_header_t h;
    uint32_t lapic_addr;
    uint32_t flags;
    struct record data[1];
} _packed;

void apic_init(void);
void apic_start(void);

enum
{
    LAPIC_REG_ID           = 0x20,
    LAPIC_REG_VERSION      = 0x30,
    LAPIC_REG_TPR          = 0x80, /* Task priority register */
    LAPIC_REG_APR          = 0x90, /* Arbiration priority register */
    LAPIC_REG_PPR          = 0x90, /* Processor priority register */
    LAPIC_REG_EOI          = 0xA0, /* End of interrupt */
    LAPIC_REG_RRD          = 0x90, /* Remote read register */
    LAPIC_REG_DEST_FORMAT  = 0xE0,
    LAPIC_REG_SPURIOUS     = 0xF0, /* Spurrious interrupt register */
    LAPIC_REG_ISR          = 0x100, /* In-service register */
    LAPIC_REG_TMR          = 0x180, /* Trigger mode register */
    LAPIC_REG_IRR          = 0x200, /* Interrupt request register */
    LAPIC_REG_ERR_STATUS   = 0x280,
    LAPIC_REG_CMCI         = 0x2F0, /* LVT Corrected machine check interrupt */
    LAPIC_REG_ICR          = 0x300, /* Interrupt control register */
    LAPIC_REG_LVT_TMR      = 0x320,
    LAPIC_REG_LVT_THERMAL  = 0x330,
    LAPIC_REG_LVT_PMC      = 0x340, /* LVT Performance monitoring counters */
    LAPIC_REG_LVT_LINT0    = 0x350,
    LAPIC_REG_LVT_LINT1    = 0x360,
    LAPIC_REG_LVT_ERR      = 0x370,
    LAPIC_REG_TMR_INITIAL  = 0x380, /* Initial counst */
    LAPIC_REG_TMR_CUR      = 0x390, /* Current count */
    LAPIC_REG_TMR_DIV      = 0x3E0, /* Timer divide config register */

    APIC_TMR_DIV_BY_16 = 0x3,

    LAPIC_TMR_MASKED = 0x10000,
    LAPIC_TMR_PERIODIC = 0x20000,

    LAPIC_ENABLE = 0x800,

    LAPIC_SPURIOUS_ALL = 0xFF,
    LAPIC_SPURIOUS_ENABLE = 0x100
};

#endif
