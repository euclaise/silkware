#include <stdint.h>

void mmoutb(void *addr, uint8_t val)
{
    __asm__ volatile ("mov %0, %1" : "=m"(addr) : "r"(val) : "memory");
}
