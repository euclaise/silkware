#include <io.h>
#include <kern.h>
#include <memmap.h>
#include <paging.h>
#include <screen.h>
#include <panic.h>
#include <timer.h>
#include <page_alloc.h>
#include <sched.h>
#include <mp.h>
#include <kalloc.h>
#include <assert.h>

extern void *high_addr;

void return_user(void);
void user_main1(void);
void user_main2(void);

struct proc kproc = {0};

void kmain(void)
{
    int cpu_id;
    serial_init();
    init_fb();

    printf("%c Silkware%c\n\n", COLOR_GREEN, COLOR_WHITE);
    printf("Framebuffer: phys=%p virt=%p\n", screen.paddr, screen.vaddr);

    memmap_init();

    map_kern_pages();
    map_screen();
    init_cpu_early();
    flush_pages(NULL);

    printf("CPU %d (%d total)\n", cpu_id = get_cpu_data()->id, ncpus = get_ncpus());

    page_alloc_init();
    kalloc_init();

    init_cpu_local();
    printf("Remapped kernel\n");
    printf("Framebuffer mapped at: %p\n", screen.vaddr);

    arch_init();
    printf("Arch initialization complete\n");

    init_syscalls();
    printf("Syscall initialization complete\n");

    mp_init();

    proc_init();
    sched_init();
    schedule(proc_new((void *) user_main1, PAGE_SIZE), 1);
    schedule(proc_new((void *) user_main2, PAGE_SIZE), 2);
    sched_begin();
}
