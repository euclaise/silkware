#ifndef PROC_H
#define PROC_H
#include <stdint.h>
#include <flex.h>
#include <types.h>
#include <map.h>
#include <paging.h>

struct segment
{
    uintptr_t base;
    uintptr_t len;
};

struct proc
{
    pid_t pid;
    FLEX(fd_t) fsfd; /* fd->fsfd map */
    uint32_t fsid;
    map *addrs; /* phys base -> segment */
    page_tab pt;
};

extern MAP(pid_t, proc) *proc_map;
#endif
