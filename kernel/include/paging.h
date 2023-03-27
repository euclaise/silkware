#include <stdint.h>
#include <stddef.h>
void refresh_pages(void);
void map_pages(uintptr_t dst, uintptr_t src, uintptr_t length, int flags);
void map_kern_pages(void);
void map_screen(void);
void refresh_pages(void);
void *kmap_phys(void *phys, size_t len);
void kunmap(void *virt, size_t len);
