#include <proc.h>
#include <sched.h>
#include <mp.h>

void procp_init(struct proc *p)
{
    p->arch_state.rsp = STACK_TOP;
    p->arch_state.rip = PROC_ENTRY;
    p->arch_state.flags = 0x202;
}
