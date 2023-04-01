#include <stdint.h>
#include <stddef.h>
struct memmap_entry
{
    uintptr_t base;
    uintptr_t len;
};

extern struct memmap_entry *memmap;
extern size_t memmap_len;
void memmap_init(void);
