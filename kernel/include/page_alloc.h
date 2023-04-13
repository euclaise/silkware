#include <types.h>

void page_alloc_init(void);
void *page_alloc(size_t size);
void page_free(void *b, size_t size);
void *page_realloc(void *page, size_t oldsize, size_t newsize);
void *page_zalloc(size_t size);

uintptr_t prev_pow2(uintptr_t x);
uintptr_t next_pow2(uintptr_t x);
