#include <types.h>
#include <proc.h>
#include <flex.h>

typedef int32_t fsid_t;

/* List of fs drivers indexed by fsid */
extern Flex(struct proc *) *fs_list;

typedef struct fs_inode
{
    fsid_t fsid;
    Flex(int8_t) *path;
    Flex(struct fs_inode *) *child;
} fs_inode;
