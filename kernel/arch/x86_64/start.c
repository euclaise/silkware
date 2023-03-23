#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <mem.h>
#include <kern.h>
#include <io.h>
#include "idt.h"
#include "serial.h"

__attribute__((section(".limine")))
static volatile struct limine_terminal_request term_req = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

struct limine_terminal *_term;
limine_terminal_write _write;

static int serial_ok;

void main(void);
void start(void)
{
    if (term_req.response == NULL || term_req.response->terminal_count < 1)
        freeze();

    _term = term_req.response->terminals[0];
    _write = term_req.response->write;

    idt_init();
    if (serial_init()) prints("Serial initialization failed");
    else serial_ok = 1;

    if (serial_ok)
    {
        serial_write('A');
        serial_write('B');
        serial_write('\n');
    }

    __asm__ volatile ("int $0");

    main();
}
