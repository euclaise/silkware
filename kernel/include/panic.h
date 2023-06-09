#ifndef PANIC_H
#define PANIC_H
#include <u.h>
#include <io.h>
#include <kern.h>

#define panic(...) \
    do { \
        printf("\n\n%c=== KERNEL PANIC ===\n%c", COLOR_RED, COLOR_WHITE); \
        printf("Panic in %s at %s:%d (%p)\n\n", \
                __func__, __FILE__, __LINE__, __ADDR__); \
        printf(__VA_ARGS__); \
        putc('\n'); \
        backtrace(); \
        freeze(); \
    } while (0)

#endif
