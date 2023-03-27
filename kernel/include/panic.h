#ifndef PANIC_H
#define PANIC_H
#include <util.h>
#include <io.h>
#include <kern.h>

#define panic(...) \
    do { \
        printf("\n\n=== KERNEL PANIC ===\n"); \
        printf("Panic in %s at %s:%d (%p)\n\n", \
                __func__, __FILE__, __LINE__, __ADDR__); \
        printf(__VA_ARGS__); \
        putc('\n'); \
        freeze(); \
    } while (0)

#endif
