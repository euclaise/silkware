#include <cpu.h>
#include <stddef.h>

int get_cpuid(void)
{
    int ebx;
    cpuid(1, NULL, &ebx, NULL, NULL);
    return ebx >> 24;
}

int get_ncpus(void)
{
    int ebx;
    cpuid(1, NULL, &ebx, NULL, NULL);
    return 1 + (ebx >> 16);
}
