#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <mem.h>
#include <kern.h>
#include <io.h>
#include "idt.h"
#include "serial.h"

struct limine_terminal *_term;
limine_terminal_write _write;

int serial_ok;

void main(void);
void start(void)
{

    idt_init();
    serial_ok = !serial_init();

    __asm__ volatile ("int $0");

    main();
}
