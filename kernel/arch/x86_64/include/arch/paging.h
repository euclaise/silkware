#ifndef ARCH_PAGING_H
#define ARCH_PAGING_H

#include <types.h>
#include <paging.h>
uintptr_t kvirt2phys(page_tab pml4, void *virt);

#endif
