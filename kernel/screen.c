#include <screen.h>
#include <mem.h>
#include <paging.h>

framebuffer screen;

void putpixel(uint32_t x, uint32_t y, uint32_t color)
{
    uint32_t b = color & 0xFF;
    uint32_t g = (color >> 8) & 0xFF;
    uint32_t r = (color >> 16) & 0xFF;
    uint32_t *fb = screen.address + x * (screen.bpp / 8) + y * screen.pitch;
    *fb = b << screen.blue_mask_shift
        | g << screen.green_mask_shift
        | r << screen.red_mask_shift;
}
