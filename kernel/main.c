#include <io.h>
#include <kern.h>
#include <phys_malloc.h>

void sbrk_init(void);
void main(void)
{
    serial_init();
    sbrk_init();
    printf("Hello world! - 0x123abcdef is %p\n", 0x123abcdef);

    char *r = phys_malloc(100);
    r[99] = 0;
    for (int i = 0; i < 99; ++i) r[i] = 'A';

    printf("%p\n%s", r, r);
    freeze();
}
