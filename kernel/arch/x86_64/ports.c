#include <stdint.h>

uint8_t port_in(uint16_t port)
{
    uint8_t res;
    __asm__ volatile (
            "in %1, %0"
            : "=a" (res)
            : "d" (port)
        );
    return res;
}

void port_out(uint16_t port, uint8_t x)
{
    __asm__ volatile (
            "out %1, %0"
            :
            : "d" (port), "a" (x)
        );
}
