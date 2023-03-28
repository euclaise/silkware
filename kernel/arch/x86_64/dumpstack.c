#include <stdint.h>
#include <io.h>

struct frame
{
    struct frame *rbp;
    uint64_t rip;
};

void dump_stack(void)
{
    struct frame *stk;
    __asm__ volatile ("mov %%rsp,%0" : "=r" (stk));
    printf("Stack trace:\n");


    for (int i = 0; stk && i < 5; ++i)
    {
        printf("    0x%16lx\n", stk->rip);
        stk = stk->rbp;
    }
}
