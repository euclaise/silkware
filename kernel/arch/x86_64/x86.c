#include <stdint.h>
#include <mp.h>
#include <io.h>
#include <assert.h>
#include <mp.h>

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

void init_cpu_local(void)
{
    int ebx;
    cpuid(1, NULL, &ebx, NULL, NULL);
    get_cpu_data()->id = ebx >> 24;
}
