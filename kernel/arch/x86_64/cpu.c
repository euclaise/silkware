#include <stdint.h>
#include <cpu.h>

int32_t cpuid_features(void)
{
    int32_t res;
    __asm__ volatile (
        "cpuid"
        : "=d"(res)
        : "a"(1)
        : "ebx", "ecx"
    );
    return res;
}
