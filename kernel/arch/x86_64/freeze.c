#include <io.h>

void freeze(void)
{
    printf("=== Kernel is freezing ===");
    while (1)
        __asm__ volatile (
                "cli\n"
                "hlt"
            );
}
