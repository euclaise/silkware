#include <sched.h>
#include <types.h>
#include <kalloc.h>
#include <proc.h>
#include <mp.h>
#include <assert.h>
#include <io.h>
#include <panic.h>
#include <lock.h>

#define NPRIOR (5) /* Number of priority levels */

bool sched_ready;

struct node
{
    struct node *next;
    pid_t pid;
};

struct queue
{
    struct node *start[NPRIOR];
    struct node *end[NPRIOR];
};

struct sched_cpu_queue *sched_cpu_queues;

void sched_init(void)
{
    int i;
    sched_cpu_queues = kalloc(sizeof(struct queue)*ncpus);

    for (i = 0; i < ncpus; ++i)
    {
        sched_cpu_queues[i].active = kzalloc(sizeof(struct queue));
        sched_cpu_queues[i].inactive = kzalloc(sizeof(struct queue));
        LOCK_CLEAR(&sched_cpu_queues[i].lock);
    }
}

void proc_activate(pid_t pid)
{
    struct cpu_data *cpu = get_cpu_data();
    cpu->proc_current = map_get(procmap, &pid, sizeof(pid));
    assert(cpu->proc_current != NULL);
}

static void push_unlocked(
    struct sched_cpu_queue *cpu_queue,
    struct node *new,
    int priority)
{
    if (cpu_queue->inactive->end[priority])
        cpu_queue->inactive->end[priority]->next = new;

    cpu_queue->inactive->end[priority] = new;

    if (cpu_queue->inactive->start[priority] == NULL)
        cpu_queue->inactive->start[priority] = new;

}

static pid_t pop_unlocked(struct sched_cpu_queue *cpu_queue)
{
    size_t i;
    for (i = 0; i < NPRIOR; ++i)
        if (cpu_queue->active->start[i])
        {
            struct node *node;

            node = cpu_queue->active->start[i];
            if (!node) continue;

            if (cpu_queue->active->end[i] == node)
                cpu_queue->active->end[i] = NULL;

            cpu_queue->active->start[i] = node->next;
            node->next = NULL;

            push_unlocked(cpu_queue, node, i);

            return node->pid;
        }
    return 0;
}

void schedule(pid_t pid, int priority)
{
    uint32_t n_min;
    int cpu_min;
    int cpu;

    struct node *new = kalloc(sizeof(struct node));
    new->pid = pid;
    new->next = NULL;

    for (cpu = 0, n_min = UINT32_MAX; cpu < ncpus; ++cpu)
    {
        if (sched_cpu_queues[cpu].n < n_min)
        {
            cpu_min = cpu;
            n_min = sched_cpu_queues[cpu].n;
        }
    }

    ACQUIRE(&sched_cpu_queues[cpu_min].lock);
    ++sched_cpu_queues[cpu_min].n;
    push_unlocked(&sched_cpu_queues[cpu_min], new, priority);
    RELEASE(&sched_cpu_queues[cpu_min].lock);
}

void proc_next(void)
{
    pid_t pid;
    struct queue *tmp;
    int cpuid = get_cpu_data()->id;

    ACQUIRE(&sched_cpu_queues[cpuid].lock);
    if ((pid = pop_unlocked(&sched_cpu_queues[cpuid])))
    {
        proc_activate(pid);

        RELEASE(&sched_cpu_queues[cpuid].lock);
        return;
    }

    tmp = sched_cpu_queues[cpuid].inactive;
    sched_cpu_queues[cpuid].inactive = sched_cpu_queues[cpuid].active;
    sched_cpu_queues[cpuid].active = tmp;

    if ((pid = pop_unlocked(&sched_cpu_queues[cpuid])) == 0)
        panic("No processes available!");
    proc_activate(pid);
    RELEASE(&sched_cpu_queues[cpuid].lock);
}

static bool queue_tryremove(struct queue *q, pid_t pid)
{
    size_t i;
    struct node *n;
    for (i = 0; i < NPRIOR; ++i)
    {
        struct node *prev = NULL;
        for (n = q->start[i]; n != NULL; n = n->next)
        {
            if (n->pid == pid)
            {
                if (q->end[i] == n) q->end[i] = prev;
                
                if (prev) prev->next = n->next;
                else q->start[i] = n->next;

                kfree(n);
                return true;
            }
            prev = n;
        }
    }
    return false;
}

void unschedule(pid_t pid)
{
    int cpuid = get_cpu_data()->id;

    ACQUIRE(&sched_cpu_queues[cpuid].lock);

    --sched_cpu_queues[cpuid].n;
    sched_update_duration();

    if (!queue_tryremove(sched_cpu_queues[cpuid].active, pid))
        queue_tryremove(sched_cpu_queues[cpuid].inactive, pid);

    RELEASE(&sched_cpu_queues[cpuid].lock);
}

void sched_begin(void)
{
    sched_ready = true;
    while (1) pause();
}
