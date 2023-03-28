#include <stdint.h>
#include <io.h>
#include <panic.h>
extern char _kbrk_start[], kern_end[];

static void *_kbrk;

void sbrk_init(void)
{
    _kbrk = _kbrk_start;
}

void *phys_sbrk(intptr_t inc)
{
    void *old = _kbrk;
    _kbrk += inc;
    if (_kbrk > (void *) kern_end) panic("Out of memory");
    return old;
}
