#include <miniheap.h>
#include <paging.h>
#include <panic.h>
#include <lock.h>

extern char miniheap_start[], miniheap_end[];
static char *brk = miniheap_start;
static lock_t miniheap_lock = LOCK_INIT;

void *miniheap_alloc(size_t size)
{
    void *res;
    ACQUIRE(&miniheap_lock);

    res = brk;
    brk += round_up_page(size);

    if (brk > miniheap_end) panic("Out of miniheap space");

    RELEASE(&miniheap_lock);
    return res;
}
