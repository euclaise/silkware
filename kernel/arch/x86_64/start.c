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

void *high_addr;

void main(void);
void start(void)
{
    __asm__ volatile ("mov %0, %%rsp" : : "r" (stack_top) : "memory");

    if (hhdm_req.response == NULL) freeze();

    high_addr = (void *) hhdm_req.response->offset;
    idt_init();
    main();
}
