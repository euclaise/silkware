#ifndef SCHED_H
#define SCHED_H
#include <types.h>

#define SCHED_US_PER_SWEEP (10000)

void sched_init(void);
void proc_next(void);
void schedule(pid_t pid, int priority);
void sched_begin(void);
void sched_update_duration(void);
extern uint32_t sched_n;

#endif
