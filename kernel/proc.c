#include <proc.h>
#include <sched.h>
#include <vec.h>

Vec(struct proc) *procs;

void proc_new(struct proc p)
{
    vec_push(&procs, p);
}
