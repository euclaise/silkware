#include <io.h>
#include <kern.h>

NORETURN void freeze(void)
{
    printf("%c=== Kernel is freezing ===", COLOR_RED);
    while (1) __asm__ volatile (
        "cli\n"
        "hlt"
    );
}
