#include <proc.h>
#include <sched.h>
#include <vec.h>
#include <map.h>
#include <lock.h>
#include <io.h>
#include <assert.h>

map *procmap;

pid_t pid_cur;
DEF_LOCK(pid_cur_lock);

void proc_init(void)
{
    procmap = map_new(1204);
    assert(procmap != NULL);
}

pid_t proc_new(void)
{
    struct proc p = {0};
    ACQUIRE(&pid_cur_lock);
    pid_t pid = ++pid_cur;
    RELEASE(&pid_cur_lock);

    p.pid = pid_cur;
    procp_init(&p);
    newproc_pages(&p);

    map_insert(&procmap, &pid, sizeof(pid), &p, sizeof(p));
    return p.pid;
}
