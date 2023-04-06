#include <cpu.h>
#include <assert.h>
#include <stdint.h>



void pause(void)
{
    __asm__ volatile ("pause");
}

