#include <arch/apic.h>
#include <sched.h>
#include <types.h>

void sched_update_duration(void)
{
    uint32_t us = SCHED_US_PER_SWEEP / sched_n;
    if (us == 0) us = 1;
    apic_set_duration(us);
}
