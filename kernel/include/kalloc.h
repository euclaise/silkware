#include <stddef.h>
void kalloc_init(void);
void *kalloc(size_t size);
void kfree(void *addr);
void *krealloc(void *old, size_t newsize);
