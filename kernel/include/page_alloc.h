#include <stddef.h>
#include <stdint.h>

void page_alloc_init(void);
void *page_alloc(size_t size);
void page_free(void *b, size_t size);
void *page_realloc(void *page, size_t oldsize, size_t newsize);


uintptr_t prev_pow2(uintptr_t x);
uintptr_t next_pow2(uintptr_t x);
