#include <limine.h>
#include <io.h>
#include <screen.h>
#include <panic.h>
#include "addr.h"

struct limine_framebuffer_request fb_req = {
    .id = LIMINE_FRAMEBUFFER_REQUEST
};

void init_fb(void)
{
    struct limine_framebuffer *fb = 0;
    if (fb_req.response == 0 || fb_req.response->framebuffer_count == 0)
        panic("Could not get framebuffer");

    for (int i = 0; i < (int) fb_req.response->framebuffer_count; ++i)
        if (fb == 0
                || fb_req.response->framebuffers[i]->width > fb->width
                || (fb_req.response->framebuffers[i]->width == fb->width
                    && fb_req.response->framebuffers[i]->height > fb->width)
            )
            fb = fb_req.response->framebuffers[i];

    screen.vaddr = fb->address;
    screen.paddr = H2PHYS(fb->address);
    screen.width = fb->width;
    screen.height = fb->height;
    screen.pitch = fb->pitch;
    screen.bpp = fb->bpp;
    screen.red_mask_shift = fb->red_mask_shift;
    screen.green_mask_shift = fb->green_mask_shift;
    screen.blue_mask_shift = fb->blue_mask_shift;
}
