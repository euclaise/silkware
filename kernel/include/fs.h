#include <stdint.h>
#include <proc.h>
#include <flex.h>

typedef struct fs_stat
{
    FLEX(int8_t) name;
    uint32_t ops;
    uint64_t size;
} fs_stat;

typedef FLEX(int8_t *) fs_entry_list;

/* List of fs drivers indexed by fsid */
extern FLEX(proc *) *fs_list;

typedef struct fs_entry
{
    int32_t fsid;
    FLEX(int8_t) path;
    FLEX(struct fs_entry *) child;
} fs_entry;
