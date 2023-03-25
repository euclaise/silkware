#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <mem.h>
#include <kern.h>
#include <io.h>
#include "idt.h"
#include "serial.h"

uintptr_t stack_start;
uint32_t stack_size = 0x10000;

void main(void);
void start(void)
{
    __asm__ (
        "mov %%rsp, %0"
        : "=r" (stack_start)
    );
    stack_start -= 0x10000;
    stack_start = (stack_start & ~0xFFF) + 0x1000;
    idt_init();
    main();
}
