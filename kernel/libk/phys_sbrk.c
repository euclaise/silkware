#include <stdint.h>
extern void *_kbrk_start;

static void *_kbrk;

void sbrk_init(void)
{
    _kbrk = &_kbrk_start;
}

void *phys_sbrk(intptr_t inc)
{
    void *old = _kbrk;
    _kbrk += inc;
    return old;
}
