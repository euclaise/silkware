#include <io.h>
#include <kern.h>
#include <memmap.h>
#include <paging.h>
#include <screen.h>
#include <panic.h>
#include <lai/core.h>
#include <lai/helpers/sci.h>
#include <timer.h>
#include <page_alloc.h>
#include <proc.h>
#include <mp.h>

extern void *high_addr;

void call_user(void);
void main(void)
{
    struct proc p1 = {0};
    serial_init();
    init_fb();

    printf("Silkware\n\n");
    printf("CPU #%d of %d\n", get_cpuid(), get_ncpus());
    printf("Framebuffer: phys=%p virt=%p\n", screen.paddr, screen.vaddr);
    memmap_init();
    map_kern_pages();
    map_screen();
    refresh_pages(NULL);
    page_alloc_init();

    printf("Remapped kernel\n");
    printf("Framebuffer mapped at: %p\n", screen.vaddr);

    arch_init();
    printf("Arch initialization complete\n");

    init_syscalls();
    printf("Syscall initialization complete\n");
    screen.vaddr = 0;

    newproc_pages(&p1);
    refresh_pages(p1.pt);
    call_user();
    __asm__ ("int $0");
}
