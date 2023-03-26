#include <stdint.h>
#include <limine.h>

typedef struct limine_framebuffer framebuffer;
extern framebuffer screen;
void init_fb(void);
void putpixel(uint32_t x, uint32_t y, uint32_t color);
void putchar(uint32_t x, uint32_t y, char c);
