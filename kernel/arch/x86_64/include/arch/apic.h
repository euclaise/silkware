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

#endif
