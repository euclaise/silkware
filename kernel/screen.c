#include <screen.h>
#include <mem.h>
#include <paging.h>

framebuffer screen;

void putpixel(int x, int y, int color)
{
    int colorsize = screen.bpp / 24;
    colorsize = colorsize == 0 ? 1 : colorsize;
    uint32_t where = x * colorsize * 3 + y * screen.pitch;

    int b = color & 255;
    int g = (color >> 8) & 255;
    int r = (color >> 16) & 255;

    uint8_t *fb = screen.address;

    memcpy(&fb[where], &b, colorsize);
    memcpy(&fb[where + 1], &g, colorsize);
    memcpy(&fb[where + 2], &r, colorsize);
}
