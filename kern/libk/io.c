#include <io.h>
#include <stdint.h>

void prints(char *s)
{
    do { putc(*s++); } while (*s);
}

void printx(uintptr_t x)
{
    const char hex_chars[] = "0123456789abcdef";
    char digits[20] = {0};
    char *p = digits + 20;

    *p-- = 0;
    do
    {
        *p-- = hex_chars[x % 0x10];
        x /= 0x10;
    } while (x);

    prints(p + 1);
}
