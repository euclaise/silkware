#include <io.h>
#include <types.h>
#include <kern.h>
#include <lock.h>

#define NANOPRINTF_IMPLEMENTATION
#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#include "nanoprintf.h"

static void putc_ctx(int c, void *ctx)
{
    (void)ctx;
    putc(c);
}

lock_t printf_lock = LOCK_INIT;
void printf(char *f, ...)
{
    va_list a;
    va_start(a, f);
    ACQUIRE(&printf_lock);
    npf_vpprintf(putc_ctx, NULL, f, a);
    RELEASE(&printf_lock);
    va_end(a);
}
