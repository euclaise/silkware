#include <types.h>
#include <io.h>
#include <panic.h>
#include <sched.h>
#include <mp.h>
#include <page_alloc.h>
#include <ctx.h>

void syscall_test(int i)
{
    printf("Hello from userspace! %d\n", i);
}

void syscall_exit(void)
{
    size_t i;
    struct cpu_data *cpu = get_cpu_data();
    unschedule(cpu->proc_current->pid);
    for (i = 0; i < cpu->proc_current->segs->n; ++i)
        page_free(
            cpu->proc_current->segs->item[i].kvirt,
            cpu->proc_current->segs->item[i].len
        );
    map_free(cpu->proc_current->addrs);
    page_free(cpu->proc_current->pt, PAGE_SIZE);
    map_del(procmap, &cpu->proc_current->pid, sizeof(pid_t));

    cpu->proc_current = NULL;
    ctx_switch(NULL);
}

void syscall_inval(void)
{
    panic("Invalid syscall");
}
