#include <stdint.h>
#include <io.h>

struct frame
{
    struct frame *rbp;
    uint64_t rip;
} __attribute__((packed));

void dump_stack(void)
{
    struct frame *stk;
    __asm__ volatile ("mov %%rbp,%0" : "=r" (stk));
    printf("Stack trace:\n");


    for (int i = 0; stk >= (struct frame *) 0xffffffff80000000 && i < 5; ++i)
    {
        printf("    %p\n", stk->rip);
        stk = stk->rbp;
    }
}
