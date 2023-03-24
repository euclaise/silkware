#include <io.h>
#include <kern.h>

void main(void)
{
    serial_init();
    printf("Hello world! - 0x123abcdef is %p\n", 0x123abcdef);
    freeze();
}
