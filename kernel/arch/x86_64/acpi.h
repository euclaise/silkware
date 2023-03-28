#include <stdint.h>
#include <acpispec/tables.h>

extern acpi_rsdp_t rsdp;

typedef struct rsdt_t
{
    acpi_header_t h;
    uint32_t p[];
} __attribute__((packed)) rsdt_t;
