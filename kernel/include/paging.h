#ifndef PAGING_H
#define PAGING_H
#include <types.h>
#include <map.h>
#include <addrspace.h>
#include <proc.h>

#define PAGE_PRESENT  (1 << 0)
#define PAGE_WRITABLE (1 << 1)
#define PAGE_USER     (1 << 2)
#define PAGE_NX       (1 << 3)
#define PAGE_NOCACHE  (1 << 4)

uintptr_t round_up_page(uintptr_t page);
void flush_pages(struct addrspace *space);
void *map_anon(uintptr_t phys, uintptr_t len, int flags);
void unmap_pages(void *virt, size_t len);
void addrspace_free(void *addr);
void proc_pages_init(struct proc *p, void *start, size_t len);
void map_screen(void);
void map_kern_pages(void);
void *premap_anon(uintptr_t phys, uintptr_t len, int flags);

#endif
