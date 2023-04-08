#include <stdint.h>
#include <syscall.h>
#include <io.h>
#include <panic.h>

void syscall_test(int i)
{
    printf("Hello from userspace! %d\n", i);
}

void syscall_inval(void)
{
    panic("Invalid syscall");
}
