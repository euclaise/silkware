#include <limine.h>
#include <screen.h>
#include "serial.h"

extern int serial_ok;
void putc(char c)
    {
    static int x = 0;
    static int y = 10;
    if (serial_ok) serial_write(c);
    if (screen.address)
    {
        x += 10;
        if (c == '\n' || x + 10 > (int) screen.width)
        {
            x = 0;
            y += 10;
            if (y > (int) screen.height) y = 10;
        }
        if (c != '\n') putchar(x, y, c);
    }
}
