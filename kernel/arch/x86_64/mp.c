#include <arch/cpu.h>
#include <types.h>
#include <mp.h>
#include <page_alloc.h>
#include <arch/paging.h>
#include <io.h>
#include <arch/x86.h>
#include <rand.h>
#include <arch/apic.h>
#include <kern.h>
#include <assert.h>
#include <limine.h>
#include <miniheap.h>
#include <arch/addr.h>

struct limine_smp_request smp_req = { .id = LIMINE_SMP_REQUEST };

static bool ready;
static volatile int cpus_ready = 1;

static int get_cpuid_cpuid(void)
{
    int ebx;
    cpuid(1, NULL, &ebx, NULL, NULL);
    return ebx >> 24;
}

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
    cpu->id = get_cpuid_cpuid();
    ready = 1;
}

void mp_pages(void);
void apic_start(void);

void mp_start(void)
{
    mp_pages();
    init_cpu_local();

    idt_init();
    gdt_init();
    rand_init();
    apic_start();

    init_syscalls();

    ++cpus_ready;

    printf("CPU %d ready", get_cpuid());
    while (1) pause();
}

void _mp_start(struct limine_smp_info *info);
extern void *high_addr;

void mp_init(void)
{
    int i;

    smp_req.response = kmap_phys(
         H2PHYS(smp_req.response),
        sizeof(*smp_req.response)
    );
    assert_eq(smp_req.response->cpu_count, (uint64_t) ncpus);

    smp_req.response->cpus = kmap_phys(
        H2PHYS(smp_req.response->cpus),
        ncpus*sizeof(struct limine_smp_info *)
    );
    for (i = 0; i < ncpus; ++i)
    {
        struct limine_smp_info *cpu = smp_req.response->cpus[i];
        cpu = kmap_phys(H2PHYS(cpu), sizeof(*cpu));
        __atomic_store_n(&cpu->goto_address, _mp_start, __ATOMIC_RELEASE);
    }
    while (cpus_ready != ncpus);
}
