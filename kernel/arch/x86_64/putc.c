#include <limine.h>
#include <screen.h>
#include <io.h>
#include "serial.h"

extern int serial_ok;

static int color = COLOR_WHITE;

void putc(char c)
{
    static int x = 0;
    static int y = 10;


    if (c == COLOR_RED)
    {
        color = COLOR_RED;
        return;
    }
    else if (c == COLOR_WHITE)
    {
        color = COLOR_WHITE;
        return;
    }

    if (serial_ok) serial_write(c);
    if (screen.vaddr)
    {
        x += 10;
        if (c == '\n' || x + 10 > (int) screen.width)
        {
            x = 0;
            y += 17;
            if (y > (int) screen.height) y = 17;
        }

        if (c != '\n')
            switch (color)
            {
            case COLOR_RED:
                putchar(x, y, c, 0xFF0000);
                break;
            case COLOR_WHITE:
            default:
                putchar(x, y, c, 0xFFFFFF);
            }
    }
}
