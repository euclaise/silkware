#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <mem.h>
#include <kern.h>
#include <io.h>
#include "idt.h"

__attribute__((section(".limine")))
static volatile struct limine_terminal_request term_req = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

__attribute__((section(".limine")))
static volatile struct limine_hhdm_request hhdm_req = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

struct limine_terminal *_term;
limine_terminal_write _write;

uintptr_t _kern_offset;

void main(void);
void start(void)
{
    if (term_req.response == NULL || term_req.response->terminal_count < 1)
        freeze();

    _term = term_req.response->terminals[0];
    _write = term_req.response->write;

    if (hhdm_req.response == NULL)
    {
        prints("Failed to get HHDM address!");
        freeze();
    }

    _kern_offset = hhdm_req.response->offset;

    idt_init();

    __asm__ volatile ("int $0");

    main();
}
