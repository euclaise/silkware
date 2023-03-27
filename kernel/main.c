#include <io.h>
#include <kern.h>
#include <memmap.h>
#include <paging.h>
#include <screen.h>

extern void *high_addr;
void main(void)
{
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
    panic("Done");
}
