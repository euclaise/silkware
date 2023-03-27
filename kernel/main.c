#include <io.h>
#include <kern.h>
#include <memmap.h>
#include <paging.h>
#include <screen.h>
#include <panic.h>
#include <lai/core.h>

extern void *high_addr;

void thing(void)
{
}


void main(void)
{
    LAI_CLEANUP_STATE lai_state_t state;
    serial_init();
    sbrk_init();
    init_fb();

    printf("Silkware\n\n");
    printf("Framebuffer: phys=%p virt=%p\n", screen.paddr, screen.vaddr);
    printf("High: %p\n", high_addr);
    memmap_init();
    map_screen();
    map_kern_pages();
    refresh_pages();

    printf("Remapped kernel\n");
    printf("Framebuffer mapped at: %p\n", screen.vaddr);

    arch_init();

    lai_init_state(&state);

    __asm__ ("int $0");
    panic("Done");
}
