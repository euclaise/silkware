#include <types.h>
#include <limine.h>
#include <mem.h>
#include <kern.h>
#include <io.h>
#include <paging.h>
#include <kalloc.h>
#include <map.h>
#include <panic.h>
#include <acpi.h>
#include <arch/apic.h>
#include <arch/x86.h>
#include <arch/serial.h>
#include <arch/addr.h>
#include <arch/hpet.h>
#include <rand.h>
#include <lai/helpers/sci.h>

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

    kmain();
}


void gdt_meta_init(void);

void arch_init(void)
{
    idt_init();
    gdt_meta_init();
    gdt_init();
    printf("ACPI Revision: %d\n", xsdp.revision);
    acpi64 = xsdp.revision >= 2 && xsdp.xsdt;
    hpet_init();
    rand_init();
    lai_set_acpi_revision(xsdp.revision);
    lai_create_namespace();
    lai_enable_acpi(1);
    apic_init();
}
