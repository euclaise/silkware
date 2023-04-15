#include <arch/cpu.h>
#include <stddef.h>
#include <mp.h>
#include <page_alloc.h>
#include <arch/paging.h>
#include <io.h>

static bool ready;

int get_cpuid(void)
{
    if (unlikely(!ready))
    {
        int ebx;
        cpuid(1, NULL, &ebx, NULL, NULL);
        return ebx >> 24;
    }
    return get_cpu_data()->id;
}

int get_ncpus(void)
{
    int ebx;
    cpuid(1, NULL, &ebx, NULL, NULL);
    return ebx >> 16;
}

void init_cpu_local(void)
{
    struct cpu_data *cpu = get_cpu_data();
    cpu->id = get_cpuid();
    ready = 1;
}
