#include <stdint.h>
typedef struct
{
    uintptr_t base;
    uintptr_t len;
} memmap_entry;

extern memmap_entry *memmap;
extern int memmap_len;
void memmap_init(void);