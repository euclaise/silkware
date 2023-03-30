#include <stdint.h>
#include <stddef.h>

typedef void *page_tab;

void refresh_pages(page_tab tab);
void map_pages(uintptr_t dst, uintptr_t src, uintptr_t length, int flags);
void map_kern_pages(page_tab tab);
void map_screen(void);
void *kmap_phys(void *phys, size_t len);
void kunmap(void *virt, size_t len);
page_tab newproc_pages(void);
