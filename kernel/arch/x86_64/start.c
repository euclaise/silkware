#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <mem.h>
#include <kern.h>
#include <io.h>
#include "idt.h"
#include "serial.h"

extern char stack_top[];

void main(void);
void start(void)
{
    __asm__ volatile ("mov %0, %%rsp" : : "r" (stack_top) : "memory");

    idt_init();
    main();
}
