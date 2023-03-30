#include <stdint.h>

uintptr_t round_up_page(uintptr_t x)
{
    return (x + 0xFFF) & ~0xFFF;
}
