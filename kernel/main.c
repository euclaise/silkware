#include <io.h>
#include <kern.h>
#include <memmap.h>
#include <paging.h>
#include <screen.h>
#include <panic.h>
#include <lai/core.h>
#include <lai/helpers/sci.h>
#include <timer.h>
#include <page_alloc.h>

extern void *high_addr;

void main(void)
{
    serial_init();
    init_fb();

    printf("Silkware\n\n");
    printf("Framebuffer: phys=%p virt=%p\n", screen.paddr, screen.vaddr);
    printf("High: %p\n", high_addr);
    memmap_init();
    map_kern_pages(NULL);
    map_screen();
    refresh_pages(NULL);
    page_alloc_init();

    printf("Remapped kernel\n");
    printf("Framebuffer mapped at: %p\n", screen.vaddr);

    arch_init();
    printf("Arch initialization complete\n");

    newproc_pages();
    panic("Done");
}
