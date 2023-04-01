#include <stdint.h>
#include <stddef.h>

typedef uintptr_t *page_tab;
extern page_tab cur_page_tab;

extern uintptr_t end_pos;

void refresh_pages(page_tab tab);
void map_pages_default(uintptr_t dst, uintptr_t src, uintptr_t length);
void map_kern_pages(void);
void map_screen(void);
void *kmap_phys(void *phys, size_t len);
void kunmap(void *virt, size_t len);
page_tab newproc_pages(void);
intptr_t round_up_page(uintptr_t x);
