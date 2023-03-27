#include <stdint.h>
#include <phys_malloc.h>
#include <mem.h>
#include <io.h>
#include <memmap.h>
#include <limine.h>
#include <assert.h>
#include <screen.h>
#include <panic.h>
#include <flex.h>
#include <stddef.h>
#include "addr.h"

struct limine_kernel_address_request kern_addr_req = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST
};

#define PAGE_PRESENT  1
#define PAGE_WRITABLE (1 << 1)
#define PAGE_USER     (1 << 2)
#define PAGE_LARGE    (1 << 7)
#define PAGE_NOEXEC   (1ULL << 63)

typedef uintptr_t pml4e_t;
typedef uintptr_t pdpte_t;
typedef uintptr_t pde_t;
typedef uintptr_t pte_t;

typedef pml4e_t *pml4_t;
typedef pdpte_t *pdpt_t;
typedef pde_t   *pd_t;
typedef pte_t   *pt_t;

pml4e_t pml4[512]  __attribute__((aligned(0x1000)));

extern char kern_load[], kern_end[];

#define K2PHYS(x) (void *)( \
        (uintptr_t)(x) - kern_addr_req.response->virtual_base \
        + kern_addr_req.response->physical_base)

#define PHYS2K(x) (void *)( \
        (uintptr_t)(x) - kern_addr_req.response->physical_base \
        + kern_addr_req.response->virtual_base)

void refresh_pages(void)
{
    __asm__ (
        "mov %0, %%cr3\n"
        :
        : "r" (K2PHYS(pml4))
        : "memory"
    );
}

/* 
 * Maps the pages from src to src+length to the virtual addresses of dst to
 * dst+length, with flags as specified by the flags parameter.
 *
 * dst is the target virtual address, and src is the source physical address.
 * They will be rounded down to the nearest page.
 * Length will be rounded up.
 */
void map_pages(uintptr_t dst, uintptr_t src, uintptr_t length, int flags)
{
    uintptr_t dst_end = (dst + length + 0xFFF) & ~0xFFF;
    uintptr_t src_end = (src + length + 0xFFF) & ~0xFFF;

    while (dst < dst_end && src < src_end) {
        int pml4_idx = (dst >> 39) & 0x1FF;
        int pdpt_idx = (dst >> 30) & 0x1FF;
        int pd_idx   = (dst >> 21) & 0x1FF;
        int pt_idx   = (dst >> 12) & 0x1FF;

        if (!(pml4[pml4_idx] & PAGE_PRESENT))
            pml4[pml4_idx] = (uintptr_t) K2PHYS(phys_valloc(0x1000))
                | PAGE_PRESENT | PAGE_WRITABLE;

        pdpt_t pdpt = PHYS2K(pml4[pml4_idx] & ~0xFFF);
        if (!(pdpt[pdpt_idx] & PAGE_PRESENT))
            pdpt[pdpt_idx] = (uintptr_t) K2PHYS(phys_valloc(0x1000)) |
                PAGE_PRESENT | PAGE_WRITABLE;

        pd_t pd = PHYS2K(pdpt[pdpt_idx] & ~0xFFF);
        if (!(pd[pd_idx] & PAGE_PRESENT))
            pd[pd_idx] = (uintptr_t) K2PHYS(phys_valloc(0x1000)) |
                PAGE_PRESENT | PAGE_WRITABLE;

        pt_t pt = (pt_t) PHYS2K(pd[pd_idx] & ~0xFFF);
        pt[pt_idx] = (src & ~0xFFF) | flags;

        dst += 0x1000;
        src += 0x1000;
    }
}


static inline uintptr_t round_up_page(uintptr_t x)
{
    return (x + 0x1000 - 1) & ~(0x1000 - 1);
}

uintptr_t end_pos;
void map_kern_pages(void)
{
    if (kern_addr_req.response == NULL)
        panic("Could not get kernel physical addr");

    map_pages(
        (uintptr_t) kern_load,
        (uintptr_t) K2PHYS(kern_load),
        kern_end - kern_load,
        PAGE_PRESENT | PAGE_WRITABLE
    );
    end_pos = round_up_page((uintptr_t) kern_end);
}


void map_screen(void)
{
    size_t len =  screen.pitch * (screen.bpp/8) * screen.height;
    map_pages((uintptr_t) end_pos,
            (uintptr_t) screen.paddr,
            len,
            PAGE_PRESENT | PAGE_WRITABLE);
    screen.vaddr = (void *) end_pos;
    end_pos = round_up_page((uintptr_t) end_pos + len);
}

void *kmap_phys(void *phys, size_t len)
{
    void *res = (void *) end_pos;
    map_pages((uintptr_t) end_pos,
            (uintptr_t) phys,
            len,
            PAGE_PRESENT | PAGE_WRITABLE);
    end_pos = round_up_page((uintptr_t) end_pos + len);

    for (void *p = res; p < (void *) end_pos; p += 0x1000)
        __asm__ volatile ("invlpg (%0)" : : "b" (p) : "memory" );

    return res;
}

void kunmap(void *virt, size_t len)
{
    map_pages((uintptr_t) virt, 0, len, 0);
    for (void *p = virt; p < (void *) virt + len; p += 0x1000)
        __asm__ volatile ("invlpg (%0)" : : "b" (p) : "memory" );
}
