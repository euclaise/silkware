#include <stdint.h>
#include <proc.h>
#include <syscall.h>
#include <arch_proc.h>
#include <assert.h>

#define EFER  (0xC0000080)
#define STAR  (0xC0000081)
#define LSTAR (0xC0000082)
#define FMASK (0xC0000084)

#define EFLAGS_CARRY (1 << 0)
#define EFLAGS_TRAP (1 << 8)
#define EFLAGS_INTERRUPT (1 << 9)
#define EFLAGS_DIRECTION (1 << 10)
#define EFLAGS_NESTED_TASK (1 << 14)
#define EFLAGS_ALIGNMENT (1 << 18)

uint64_t rdmsr(uint32_t msr)
{
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));

    return ((uint64_t) high << 32) | low;
}

void wrmsr(uint32_t msr, uint64_t addr)
{
    uint32_t low = addr & 0xFFFFFFFF;
    uint32_t high = addr >> 32;

    __asm__ volatile ("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}

void syscall_entry();
void init_syscalls(void)
{
    uint64_t kern_cs = 0x08;
    uint64_t user_cs = 0x18;
    wrmsr(EFER, rdmsr(EFER) | 1); /* Enable syscall instruction */

    assert(rdmsr(EFER) & 1);

    wrmsr(FMASK,
        EFLAGS_CARRY
        | EFLAGS_TRAP
        | EFLAGS_INTERRUPT
        | EFLAGS_DIRECTION
        | EFLAGS_NESTED_TASK
        | EFLAGS_ALIGNMENT
    );
    wrmsr(STAR, (user_cs << 48) | (kern_cs << 32));

    wrmsr(LSTAR, (uint64_t) syscall_entry);
}