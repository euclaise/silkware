#include <screen.h>
#include <mem.h>
#include <paging.h>

framebuffer screen;

void putpixel(int x, int y, int color)
{
    int colorsize = screen.bpp / 24;
    int b = color & 255;
    int g = (color >> screen.bpp / 3) & 255;
    int r = (color >> (screen.bpp / 3) * 2) & 255;

    uint8_t *fb = screen.address;

    uint32_t where = x * colorsize * 4 + y * screen.pitch;

    memcpy(&fb[where], &b, colorsize);
    where += colorsize;
    memcpy(&fb[where], &g, colorsize);
    where += colorsize;
    memcpy(&fb[where], &r, colorsize);
}
