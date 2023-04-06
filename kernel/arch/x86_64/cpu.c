#include <cpu.h>
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

void pause(void)
{
    __asm__ volatile ("pause");
}
