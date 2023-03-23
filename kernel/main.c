#include <io.h>
#include <kern.h>

void main(void)
{
    prints("Hello world!\n");
    printx(0x123abcdef);
    freeze();
}
