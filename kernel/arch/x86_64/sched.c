#include <arch/apic.h>
#include <sched.h>
#include <types.h>
#include <mp.h>

void sched_update_duration(void)
{
    int cpuid = get_cpuid();
    uint32_t us;
    int n = sched_cpu_queues[cpuid].n;
    us = SCHED_US_PER_SWEEP / (n == 0 ? 1 : n);
    if (us == 0) us = 1;
    apic_set_duration(us);
}
