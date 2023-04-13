#include <proc.h>
#include <sched.h>
#include <mp.h>

void proc_init(struct proc *p)
{
    p->arch_state.rsp = 0xFFFFF000 - 16;
    p->arch_state.rip = 0x100000;
}
void proc_activate(struct proc *p)
{
    get_cpu_data()->proc_current = *p;
}
