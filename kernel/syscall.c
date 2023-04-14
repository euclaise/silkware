#include <types.h>
#include <io.h>
#include <panic.h>
#include <sched.h>

void syscall_test(int i)
{
    printf("Hello from userspace! %d\n", i);
}

void syscall_wait(void)
{
    proc_next();
}

void syscall_inval(void)
{
    panic("Invalid syscall");
}
