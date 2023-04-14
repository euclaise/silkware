#include <sched.h>
#include <types.h>
#include <kalloc.h>
#include <proc.h>
#include <mp.h>
#include <assert.h>
#include <io.h>
#include <panic.h>

#define NPRIOR (5) /* Number of priority levels */

bool sched_ready;
uint32_t sched_n; /* TODO: Make this CPU-local */

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

static struct
{
    struct queue *active;
    struct queue *inactive;
} queues;

void sched_init(void)
{
    queues.active = kzalloc(sizeof(struct queue));
    queues.inactive = kzalloc(sizeof(struct queue));
}

void proc_activate(pid_t pid)
{
    struct cpu_data *cpu = get_cpu_data();
    cpu->proc_current = map_get(procmap, &pid, sizeof(pid));
    assert(cpu->proc_current != NULL);
}

static void push_end(struct node *new, int priority)
{
    if (queues.inactive->end[priority])
        queues.inactive->end[priority]->next = new;

    queues.inactive->end[priority] = new;

    if (queues.inactive->start[priority] == NULL)
        queues.inactive->start[priority] = new;
}

void schedule(pid_t pid, int priority)
{
    struct node *new = kalloc(sizeof(struct node));
    ++sched_n;
    sched_update_duration();

    new->pid = pid;
    new->next = NULL;
    push_end(new, priority);
}

static pid_t sched_pop(void)
{
    size_t i;
    for (i = 0; i < NPRIOR; ++i)
        if (queues.active->start[i])
        {
            struct node *node;

            node = queues.active->start[i];
            if (!node) continue;

            if (queues.active->end[i] == node) queues.active->end[i] = NULL;

            queues.active->start[i] = node->next;
            node->next = NULL;

            push_end(node, i);

            return node->pid;
        }
    return 0;
}

void proc_next(void)
{
    pid_t pid;
    struct queue *tmp;

    if ((pid = sched_pop()))
    {
        proc_activate(pid);
        printf("A: %d\n", pid);
        return;
    }

    tmp = queues.inactive;
    queues.inactive = queues.active;
    queues.active = tmp;

    if ((pid = sched_pop()) == 0) panic("No processes available!");
    proc_activate(pid);
    printf("A: %d\n", pid);
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
    --sched_n;
    sched_update_duration();

    if (!queue_tryremove(queues.active, pid))
        queue_tryremove(queues.inactive, pid);
}

void sched_begin(void)
{
    sched_ready = true;
    while (1) pause();
}
