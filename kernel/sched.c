#include <sched.h>
#include <types.h>
#include <kalloc.h>
#include <proc.h>
#include <mp.h>
#include <assert.h>
#include <io.h>

struct readyqueue
{
    struct readyqueue *next;
    pid_t pid;
};

static struct
{
    struct readyqueue *active[3];
    struct readyqueue *inactive[3];
} queues;

void proc_activate(pid_t pid)
{
    struct cpu_data *cpu = get_cpu_data();
    cpu->proc_current = map_get(procmap, &pid, sizeof(pid));
    assert(cpu->proc_current != NULL);
}
