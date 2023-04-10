#include <stdint.h>
#include <proc.h>
#include <syscall.h>
#include <arch/proc.h>
#include <assert.h>
#include <arch/x86.h>

#define FMASK (0xC0000084)

#define EFLAGS_CARRY (1 << 0)
#define EFLAGS_TRAP (1 << 8)
#define EFLAGS_INTERRUPT (1 << 9)
#define EFLAGS_DIRECTION (1 << 10)
#define EFLAGS_NESTED_TASK (1 << 14)
#define EFLAGS_ALIGNMENT (1 << 18)


void syscall_entry();
void init_syscalls(void)
{
    uint64_t kern_cs = 0x08;
    uint64_t user_cs = 0x18;
    wrmsr(MSR_EFER, rdmsr(MSR_EFER) | 1); /* Enable syscall instruction */

    assert(rdmsr(MSR_EFER) & 1);

    wrmsr(MSR_FMASK,
        EFLAGS_CARRY
        | EFLAGS_TRAP
        | EFLAGS_INTERRUPT
        | EFLAGS_DIRECTION
        | EFLAGS_NESTED_TASK
        | EFLAGS_ALIGNMENT
    );
    wrmsr(MSR_STAR, (user_cs << 48) | (kern_cs << 32));

    wrmsr(MSR_LSTAR, (uint64_t) syscall_entry);
}
