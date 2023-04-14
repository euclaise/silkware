#ifndef PROC_H
#define PROC_H
#include <stdint.h>
#include <flex.h>
#include <types.h>
#include <map.h>
#include <paging.h>
#include <arch/proc.h>
#include <map.h>

#define SEG_R (1 << 0)
#define SEG_W (1 << 1)
#define SEG_X (1 << 2)

struct segment
{
    char perms;
    void *kvirt;
    void *base;
    uintptr_t len;
};

struct proc
{
    struct arch_proc_state arch_state;
    pid_t pid;
    Flex(fd_t) *fsfd; /* fd->fsfd map */
    Flex(struct segment) *segs;
    page_tab pt;
    map *addrs;
};

extern map *procmap;

void procp_init(struct proc *p);

void proc_init(void);
void proc_activate(pid_t proc);
pid_t proc_new(void *start, size_t len);
#endif
