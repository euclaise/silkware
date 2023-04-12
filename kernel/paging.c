#include <stdint.h>
#include <paging.h>
#include <u.h>

uintptr_t round_up_page(uintptr_t x)
{
    return (x + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
}
