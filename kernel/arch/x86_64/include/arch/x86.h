#ifndef X86_H
#define X86_H

#include <stdint.h>
void cpuid(int code, int *eax, int *ebx, int *ecx, int *edx);

uint64_t rdmsr(uint32_t msr);
void wrmsr(uint32_t msr, uint64_t addr);
#endif
