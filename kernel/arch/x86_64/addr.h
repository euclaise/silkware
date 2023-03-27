#include <stdint.h>
extern void *high_addr;
#define H2PHYS(x) (void *)((uintptr_t)(x) - (uintptr_t)high_addr)
#define PHYS2H(x) (void *)((uintptr_t)(x) + (uintptr_t)high_addr)
