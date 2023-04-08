#include <limine.h>
#include <screen.h>
#include <io.h>
#include <mem.h>
#include <arch/serial.h>

extern int serial_ok;


void putc(char c)
{
    static uint32_t x = 0;
    static uint32_t y = 10;
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
        x += 10;
        if (c == '\n' || x > screen.width)
        {
            x = 0;
            if (y + 17 + 10 > screen.height)
            {
                memmove(
                    screen.vaddr,
                    screen.vaddr + screen.pitch * 17,
                    screen.pitch * y
                );
                memset(
                    screen.vaddr + screen.pitch*y,
                    0,
                    screen.pitch*(screen.height - y)
                );
            }
            else y += 17;
        }
        if (c > 33) putchar(x, y, c, color);
    }
}
