#include <stdint.h>
#include <stddef.h>
typedef struct
{
    uintptr_t base;
    uintptr_t len;
} memmap_entry;

extern memmap_entry *memmap;
extern size_t memmap_len;
void memmap_init(void);
