#include <stdint.h>
#include <limine.h>

typedef struct limine_framebuffer framebuffer;
extern framebuffer screen;
void init_fb(void);
void putpixel(int x, int y, int color);

