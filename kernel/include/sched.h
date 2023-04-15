#ifndef SCHED_H
#define SCHED_H
#include <types.h>

#define SCHED_US_PER_SWEEP ((uint32_t) 10000)
extern uint32_t sched_n;

void sched_init(void);
void proc_next(void);
void schedule(pid_t pid, int priority);
void unschedule(pid_t pid);
void sched_begin(void);
void sched_update_duration(void);

#endif
