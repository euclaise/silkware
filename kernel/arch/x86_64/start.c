#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <mem.h>
#include <kern.h>
#include <io.h>
#include "idt.h"
#include "serial.h"
#include "addr.h"
#include "acpi.h"

extern char stack_top[];

struct limine_hhdm_request hhdm_req = {
    .id = LIMINE_HHDM_REQUEST
};

struct limine_rsdp_request rsdp_req = {
    .id = LIMINE_RSDP_REQUEST
};

void *high_addr;

rsdp_desc rsdp;

void main(void);
void start(void)
{
    if (hhdm_req.response == NULL || rsdp_req.response == NULL) freeze();
    high_addr = (void *) hhdm_req.response->offset;

    rsdp = *(rsdp_desc *) H2PHYS(rsdp_req.response->address);
    main();
}

void gdt_init(void);
void arch_init(void)
{
    idt_init();
    gdt_init();
    printf("RSDT: %p\n", rsdp.addr);
}
