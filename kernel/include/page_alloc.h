#include <stddef.h>
void page_alloc_init(void);
void *page_alloc(size_t size);
void page_free(void *b, size_t size);
void *page_realloc(void *page, size_t oldsize, size_t newsize);
