#ifndef KALLOC_H
#define KALLOC_H
#include <stddef.h>
void *kalloc(size_t size);
void kfree(void *addr);
void *krealloc(void *old, size_t newsize);
void *kzalloc(size_t sz);
void kalloc_init(void);
#endif
