#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <acpispec/tables.h>

extern acpi_xsdp_t xsdp;
extern bool acpi64;
void *acpi_find(const char *sig, size_t index);
