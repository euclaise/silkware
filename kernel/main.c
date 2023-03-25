#include <io.h>
#include <kern.h>
#include <phys_malloc.h>
#include <page.h>
#include <memmap.h>

char test[10];

void sbrk_init(void);
void main(void)
{
    serial_init();
    sbrk_init();
    printf("Hello world! - 0x123abcdef is %p\n", 0x123abcdef);

    memmap_init();
    map_kern_pages();

    test[0] = 'X';
    test[1] = 0;
    printf("\n%p %s\n", test);

    freeze();
}
