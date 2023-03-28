#include <stdint.h>

uint8_t port_inb(uint16_t port)
{
    uint8_t res;
    __asm__ volatile (
            "in %1, %0"
            : "=a" (res)
            : "d" (port)
        );
    return res;
}

void port_outb(uint16_t port, uint8_t x)
{
    __asm__ volatile (
            "out %1, %0"
            :
            : "d" (port), "a" (x)
        );
}

uint16_t port_inw(uint16_t port)
{
    uint16_t res;
    __asm__ volatile (
            "in %1, %0"
            : "=a" (res)
            : "d" (port)
        );
    return res;
}

void port_outw(uint16_t port, uint16_t x)
{
    __asm__ volatile (
            "out %1, %0"
            :
            : "d" (port), "a" (x)
        );
}

uint32_t port_ind(uint16_t port)
{
    uint32_t res;
    __asm__ volatile (
            "in %1, %0"
            : "=a" (res)
            : "d" (port)
        );
    return res;
}

void port_outd(uint16_t port, uint32_t x)
{
    __asm__ volatile (
            "out %1, %0"
            :
            : "d" (port), "a" (x)
        );
}
