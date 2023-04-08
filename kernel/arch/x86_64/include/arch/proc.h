#include <stdint.h>
#include <util.h>

struct regs
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    /* Relevant registers not included:
     * - rip
     * - rsp
     * - Segment selectors are automatically handled by the TSS
     */
} _packed;

struct syscall_regs
{
    struct regs regs;
    uint64_t rsp;
} _packed;
