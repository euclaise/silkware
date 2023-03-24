#include <limine.h>
#include <phys_malloc.h>
#include <io.h>
#include <memmap.h>

struct limine_memmap_request map_req = {
    .id = LIMINE_MEMMAP_REQUEST
};

void memmap_init(void)
{
    int i;
    struct limine_memmap_response *response = map_req.response;

    if (response == NULL || response->entry_count == 0)
        panic("Could not get memory map");

    memmap_len = response->entry_count;
    memmap = phys_malloc(sizeof(memmap_entry) * memmap_len);

    printf("Memory map (length %d):\n", memmap_len);
    for (i = 0; i < memmap_len; ++i)
    {
        printf("%d %p %p\n",
                response->entries[i]->type,
                response->entries[i]->base,
                response->entries[i]->length
            );
        if (response->entries[i]->type == LIMINE_MEMMAP_USABLE &&
                response->entries[i]->length != 0)
        {
            memmap[i].base = response->entries[i]->base;
            memmap[i].len = response->entries[i]->length;
        }
        else memmap[i].base = 0;
    }
}
