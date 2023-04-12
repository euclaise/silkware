#include <arch/cpu.h>
#include <stddef.h>
#include <mp.h>

int get_cpuid(void)
{
    return get_cpu_data()->id;
}

int get_ncpus(void)
{
    int ebx;
    cpuid(1, NULL, &ebx, NULL, NULL);
    return ebx >> 16;
}
