#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <mem.h>
#include <kern.h>
#include <io.h>
#include "idt.h"
#include "serial.h"

extern char stack_top[];

struct limine_hhdm_request hhdm_req = {
    .id = LIMINE_HHDM_REQUEST
};

struct limine_rsdp_request rsdp_req = {
    .id = LIMINE_RSDP_REQUEST
};

void *high_addr;
void *rsdp_addr;

void main(void);
void start(void)
{
    __asm__ volatile ("mov %0, %%rsp" : : "r" (stack_top) : "memory");

    if (hhdm_req.response == NULL || rsdp_req.response == NULL) freeze();

    high_addr = (void *) hhdm_req.response->offset;
    rsdp_addr = (void *) (rsdp_req.response->address - high_addr);
    main();
}

void gdt_init(void);
void arch_init(void)
{
    idt_init();
    gdt_init();
    printf("RSDP: %p\n", rsdp_addr);
}
