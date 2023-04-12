#ifndef PROC_H
#define PROC_H
#include <stdint.h>
#include <flex.h>
#include <types.h>
#include <map.h>
#include <paging.h>

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
    pid_t pid;
    FLEX(fd_t) *fsfd; /* fd->fsfd map */
    FLEX(struct segment) *segs;
    page_tab pt;
    map *addrs;
};
#endif
