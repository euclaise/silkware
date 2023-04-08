#ifndef MADT_H
#define MADT_H

#include <stdint.h>
#include <util.h>
#include <acpispec/tables.h>

struct madt
{
    acpi_header_t h;
    uint32_t lapic_addr;
    uint32_t flags;
    int8_t data[1];
} _packed;

void apic_init(void);

#endif
