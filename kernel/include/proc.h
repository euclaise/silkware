#include <stdint.h>
#include <flex.h>
#include <types.h>
#include <map.h>
#include <paging.h>

typedef struct proc
{
    pid_t pid;
    FLEX(fd_t) fds;
    uint32_t fsid;
    page_tab pt;
} proc;

MAP(pid_t, proc) *proc_map;
