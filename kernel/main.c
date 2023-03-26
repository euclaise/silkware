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
    printf("Framebuffer: %p\n", screen.address);
    printf("High: %p\n", high_addr);
    memmap_init();
    map_screen();
    map_kern_pages();
    refresh_pages();

    printf("Remapped kernel\n");
    printf("Framebuffer: %p\n", screen.address);

    arch_init();
    printf("Arch initialization complete\n");
    __asm__ ("int $0");
    freeze();
}
