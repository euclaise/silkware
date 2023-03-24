#include <stdint.h>
#include <memmap.h>
#include <io.h>
#include <kern.h>

int memmap_len;
memmap_entry *memmap;

void *page_alloc(void)
{
    int i;
    void *res = 0;

    for (i = 0; i < memmap_len; ++i)
    {
        if (memmap[i].base != 0 && memmap[i].len >= 0x1000)
        {
            res = (void *) (memmap[i].base + (uintptr_t) &kern_load);
            memmap[i].base += 0x1000;
            memmap[i].len += 0x1000;
            break;
        }
    }

    if (!res) panic("page_alloc: Out of memory");
    return res;
}
