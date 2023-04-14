#ifndef SCHED_H
#define SCHED_H
#include <types.h>

void sched_init(void);
void proc_next(void);
void schedule(pid_t pid, int priority);
void sched_begin(void);

#endif
