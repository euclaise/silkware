#include <screen.h>
#include <mem.h>
#include <paging.h>
#include <font.h>

struct framebuffer screen;

void putpixel(uint32_t x, uint32_t y, uint32_t color)
{
    uint32_t b = color & 0xFF;
    uint32_t g = (color >> 8) & 0xFF;
    uint32_t r = (color >> 16) & 0xFF;
    uint32_t *fb = screen.vaddr + x * (screen.bpp / 8) + y * screen.pitch;
    *fb = b << screen.blue_mask_shift
        | g << screen.green_mask_shift
        | r << screen.red_mask_shift;
}

void putchar(uint32_t x, uint32_t y, char c, uint32_t color)
{
    const uint8_t *glyph = font + 256 + 8*c*4;

    int i, j;
    for (i = 0; i < 8; ++i)
        for (j = 0; j < 8; ++j)
        {
            putpixel(x + j, y + i, color * (glyph[i] >> (7 - j) & 1));
            putpixel(x + j, y + i + 8, color * (glyph[i + 8] >> (7 - j) & 1));

            putpixel(x + j + 8, y + i, color * (glyph[i + 16] >> (7 - j) & 1));
            putpixel(
                x + j + 8,
                y + i + 8,
                color * (glyph[i + 24] >> (7 - j) & 1)
            );
        }
}
