#include <stdint.h>
#include <limine.h>

struct framebuffer
{
    void *vaddr;
    void *paddr;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint16_t bpp;
    uint8_t red_mask_shift;
    uint8_t green_mask_shift;
    uint8_t blue_mask_shift;
};

extern struct framebuffer screen;
void init_fb(void);
void putpixel(uint32_t x, uint32_t y, uint32_t color);
void putchar(uint32_t x, uint32_t y, char c, uint32_t color);
