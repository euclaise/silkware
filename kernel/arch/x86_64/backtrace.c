#include <stdint.h>
#include <io.h>
#include <util.h>

struct frame
{
    struct frame *rbp;
    uint64_t rip;
} _packed;

void backtrace(void)
{
    struct frame *stk;
    __asm__ volatile ("mov %%rbp,%0" : "=r" (stk));
    printf("Stack trace:\n");


    for (int i = 0; stk >= (struct frame *) 0xffffffff80000000 && i < 10; ++i)
    {
        printf("    %p\n", stk->rip);
        stk = stk->rbp;
    }
}
