#ifndef SCHED_H
#define SCHED_H
#include <types.h>
#include <lock.h>

#define SCHED_US_PER_SWEEP ((uint32_t) 10000)

struct sched_cpu_queue
{
    struct queue *active;
    struct queue *inactive;
    uint32_t n;
    bool sched_update;
    lock_t lock;
};

extern struct sched_cpu_queue *sched_cpu_queues;

void sched_init(void);
void proc_next(void);
void schedule(pid_t pid, int priority);
void unschedule(pid_t pid); /* For process scheduled on the current CPU */
void sched_begin(void);
void sched_update_duration(void);

#endif
