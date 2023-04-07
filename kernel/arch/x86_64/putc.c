#include <limine.h>
#include <screen.h>
#include <io.h>
#include <mem.h>
#include <serial.h>

extern int serial_ok;


void putc(char c)
{
    static int x = 0;
    static int y = 10;
    static int color = 0xFFFFFF;


    switch (c)
    {
    case COLOR_RED:
        color = 0xFF0000;
        return;
    case COLOR_WHITE:
        color = 0xFFFFFF;
        return;
    case COLOR_GREEN:
        color = 0x00FF00;
        return;
    }

    if (serial_ok) serial_write(c);
    if (screen.vaddr)
    {
        if (c == '\n' || x + 10 > (int) screen.width)
        {
            x = 0;
            y += 17;
            if (y > (int) screen.height)
            {
                memset(
                    screen.vaddr,
                    0,
                    screen.height * screen.width * (screen.bpp / 8)
                );
                y = 17;
            }
        }

        x += 10;
        if (c > 33) putchar(x, y, c, color);
    }
}
