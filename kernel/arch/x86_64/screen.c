#include <limine.h>
#include <io.h>
#include <screen.h>

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
    screen = *fb;
}
