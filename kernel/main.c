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
    printf("Silkware\n\n");

    init_fb();
    printf("Framebuffer: %p\n", screen.address);
    printf("High: %p\n", high_addr);
    memmap_init();
    map_screen();
    map_kern_pages();
    refresh_pages();

    printf("Remapped kernel\n");
    printf("Framebuffer: %p\n", screen.address);

    for (int i = 0; i < 100; ++i)
        for (int j = 0; j < 100; ++j)
            putpixel(i, j, 0xffffffff);

    putpixel(100, 100, 0xff0000);
    freeze();
}
