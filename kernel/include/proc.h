#include <stdint.h>
#include <flex.h>
#include <types.h>
#include <map.h>
#include <paging.h>

struct proc
{
    pid_t pid;
    FLEX(fd_t) fsfd; /* fd->fsfd map */
    uint32_t fsid;
    page_tab pt;
};

MAP(pid_t, proc) *proc_map;
