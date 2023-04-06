#include <arch_proc.h>
#include <stdint.h>
#include <syscall.h>
#include <io.h>
#include <panic.h>

void syscall_test(void)
{
    printf("Hello from userspace!\n");
}

void syscall_inval(void)
{
    panic("Invalid syscall");
}
