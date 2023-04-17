#include <paging.h>
#include <mp.h>
#include <sched.h>
#include <arch/proc.h>
#include <assert.h>
#include <io.h>

void return_user_(void);

struct arch_proc_state irq2state(struct irq_frame *frame)
{
    struct arch_proc_state state;
    state.rsp = frame->sp;
    state.rip = frame->ip;
    state.flags = frame->flags;
    state.regs = frame->regs;

    return state;
}

void ctx_switch(struct irq_frame *frame)
{
    struct cpu_data *cpu = get_cpu_data();
    if (cpu->proc_current && frame)
        cpu->proc_current->arch_state = irq2state(frame);
    proc_next();

    assert(cpu->proc_current != NULL);
    printf("Switching to page table %p\n", cpu->proc_current->pt);
    refresh_pages(cpu->proc_current->pt);
    assert(cpu->proc_current->arch_state.rip);
    return_user_();
}
