#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <mem.h>
#include <kern.h>
#include <io.h>
#include <lai/core.h>
#include <lai/helpers/sci.h>
#include <paging.h>
#include "idt.h"
#include "serial.h"
#include "addr.h"
#include "acpi.h"
#include "hpet.h"

struct limine_hhdm_request hhdm_req = {
    .id = LIMINE_HHDM_REQUEST
};

struct limine_rsdp_request rsdp_req = {
    .id = LIMINE_RSDP_REQUEST
};

void *high_addr;
acpi_xsdp_t xsdp;
bool acpi64;

void main(void);
void start(void)
{
    if (hhdm_req.response == NULL || rsdp_req.response == NULL) freeze();
    high_addr = (void *) hhdm_req.response->offset;
    xsdp = *(acpi_xsdp_t *) rsdp_req.response->address;

    main();
}

void gdt_init(void);
void arch_init(void)
{
    idt_init();
    gdt_init();
    printf("ACPI Revision: %d\n", xsdp.revision);
    acpi64 = xsdp.revision >= 2 && xsdp.xsdt;
    hpet_init();
    lai_set_acpi_revision(xsdp.revision);
    lai_create_namespace();
    lai_enable_acpi(1);
}
