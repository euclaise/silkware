#include <io.h>
#include <kern.h>
#include <phys_malloc.h>
#include <page.h>
#include <memmap.h>

void sbrk_init(void);
void main(void)
{
    serial_init();
    sbrk_init();
    printf("Hello world! - 0x123abcdef is %p\n", 0x123abcdef);

    memmap_init();
    map_kern_pages();

    printf("Remapped kernel\n");

    freeze();
}
