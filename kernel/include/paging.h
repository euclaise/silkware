#ifndef PAGING_H
#define PAGING_H
#include <stdint.h>
#include <stddef.h>
#include <map.h>

typedef uintptr_t *page_tab;

extern uintptr_t end_pos;

#define PAGE_PRESENT  (1 << 0)
#define PAGE_WRITABLE (1 << 1)
#define PAGE_USER     (1 << 2)
#define PAGE_NX       (1 << 3)
#define PAGE_NOCACHE  (1 << 4)

void refresh_pages(page_tab tab);
void map_pages_pt(
    page_tab pt,
    map *addrs,
    uintptr_t dst,
    uintptr_t src,
    uintptr_t length,
    int flags
);
void map_kern_pages(void);
void map_screen(void);
void *kpremap_phys(void *phys, size_t len);
void kunmap(void *virt, size_t len);
uintptr_t round_up_page(uintptr_t x);
void *kmap_phys(uintptr_t phys, uintptr_t len);
void *kmap_phys_nocache(uintptr_t phys, uintptr_t len);

void newproc_pages(void *p, void *start, size_t len);
#endif
