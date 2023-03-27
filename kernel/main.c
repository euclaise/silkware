#include <io.h>
#include <kern.h>
#include <memmap.h>
#include <paging.h>
#include <screen.h>
#include <panic.h>
#include <lai/core.h>
#include <lai/helpers/sci.h>
#include <phys_malloc.h>

extern void *high_addr;

void main(void)
{
    LAI_CLEANUP_STATE lai_state_t state;
    serial_init();
    sbrk_init();
    init_fb();

    printf("Silkware\n\n");
    printf("Framebuffer: phys=%p virt=%p\n", screen.paddr, screen.vaddr);
    printf("High: %p\n", high_addr);
    memmap_init();
    map_kern_pages();
    map_screen();
    refresh_pages();

    printf("Remapped kernel\n");
    printf("Framebuffer mapped at: %p\n", screen.vaddr);

    arch_init();

    lai_init_state(&state);
    lai_create_namespace();
    lai_enable_acpi(1);

    __asm__ ("int $0");
    panic("Done");
}
