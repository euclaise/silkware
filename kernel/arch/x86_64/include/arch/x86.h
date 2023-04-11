#ifndef X86_H
#define X86_H

#include <stdint.h>
void cpuid(int code, int *eax, int *ebx, int *ecx, int *edx);

uint64_t rdmsr(uint32_t msr);
void wrmsr(uint32_t msr, uint64_t addr);

enum
{
    INT_TIMER = 32
};

enum
{
    MSR_APIC         = 0x1B,
    MSR_EFER         = 0xC0000080,
    MSR_STAR         = 0xC0000081,
    MSR_LSTAR        = 0xC0000082,
    MSR_COMPAT_STAR  = 0xC0000083,
    MSR_FMASK        = 0xC0000084,
    MSR_FS_BASE      = 0xC0000100,
    MSR_GS_BASE      = 0xC0000101,
    MSR_KERN_GS_BASE = 0xc0000102,
};

#endif
