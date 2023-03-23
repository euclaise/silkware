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

uint8_t port_out(uint16_t port, uint8_t x)
{
    uint8_t res;
    __asm__ volatile (
            "out %1, %0"
            :
            : "d" (port), "a" (x)
        );
    return res;
}
