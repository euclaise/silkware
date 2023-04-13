#ifndef SYSCALL_H
#define SYSCALL_H

#include <types.h>
struct syscall_state
{
    uint8_t num;
};

void syscall_main(struct syscall_state state);
#endif
