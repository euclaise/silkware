#include <stdint.h>
#include <proc.h>
#include <flex.h>

typedef int32_t fsid_t;

/* List of fs drivers indexed by fsid */
extern FLEX(proc *) *fs_list;

typedef struct fs_inode
{
    fsid_t fsid;
    FLEX(int8_t) path;
    FLEX(struct fs_inode *) child;
} fs_inode;
