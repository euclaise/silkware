#include <stdint.h>
#include <mp.h>
#include <io.h>
#include <assert.h>

void cpuid(int code, int *eax, int *ebx, int *ecx, int *edx)
{
    int reax, rebx, recx, redx;
    __asm__ volatile (
        "cpuid"
        : "=a" (reax), "=b" (rebx), "=c" (recx), "=d"(redx)
        : "a"(code)
    );

    if (eax) *eax = reax;
    if (ebx) *ebx = rebx;
    if (ecx) *ecx = recx;
    if (edx) *edx = redx;
}

uint64_t rdmsr(uint32_t msr)
{
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));

    return ((uint64_t) high << 32) | low;
}

void wrmsr(uint32_t msr, uint64_t x)
{
    uint32_t low = x & 0xFFFFFFFF;
    uint32_t high = x >> 32;

    __asm__ volatile ("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}


extern struct cpu_data cpu_main;
void init_cpu_local(void)
{
    void *res;
    cpu_main.self = &cpu_main;
    wrmsr(0xC0000102 /* kernel gs base */, (uint64_t) &cpu_main);
    __asm__ volatile ("swapgs");
    __asm__ volatile ("mov %%gs:0, %0" : "=r"(res));
}

struct cpu_data *get_cpu_data(void)
{
    struct cpu_data *res;
    __asm__ volatile ("mov %%gs:0, %0" : "=r"(res));
    return res;
}
