#include <miniheap.h>
#include <paging.h>
#include <panic.h>

extern char miniheap_start[], kern_end[];
static char *brk = miniheap_start;

void *miniheap_alloc(size_t size)
{
    void *res = brk;
    brk += round_up_page(size);

    if (brk > kern_end) panic("Out of miniheap space");
    return res;
}
