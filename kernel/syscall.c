#include <arch_proc.h>
#include <stdint.h>
#include <syscall.h>
#include <io.h>
#include <panic.h>

void sys_test(void)
{
    printf("Hello from userspace!");
}

void syscall_main(struct syscall_state state)
{
    if (state.num != 0) panic("Unknown syscall!");
    sys_test();
}
