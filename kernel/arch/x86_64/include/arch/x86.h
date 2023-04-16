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

#define  MSR_APIC         (0x1B)
#define  MSR_EFER         (0xC0000080)
#define  MSR_STAR         (0xC0000081)
#define  MSR_LSTAR        (0xC0000082)
#define  MSR_COMPAT_STAR  (0xC0000083)
#define  MSR_FMASK        (0xC0000084)
#define  MSR_FS_BASE      (0xC0000100)
#define  MSR_GS_BASE      (0xC0000101)
#define  MSR_KERN_GS_BASE (0xc0000102)

void gdt_init(void);
void idt_init(void);

#endif
