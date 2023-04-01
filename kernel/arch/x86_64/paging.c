#include <u.h>
#include <stdint.h>
#include <paging.h>
#include <mem.h>
#include <io.h>
#include <limine.h>
#include <assert.h>
#include <screen.h>
#include <panic.h>
#include <stddef.h>
#include <miniheap.h>
#include "addr.h"

struct limine_kernel_address_request kern_addr_req = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST
};

static struct limine_kernel_address_response kern_resp;

page_tab cur_page_tab;

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

pml4e_t kpml4[512]  __attribute__((aligned(PAGE_SIZE)));

extern char kern_load[], kern_end[];

#define K2PHYSK(x) (void *)( \
        (uintptr_t)(x) - kern_resp.virtual_base \
        + kern_resp.physical_base)

#define PHYSK2VK(x) (void *)( \
        (uintptr_t)(x) - kern_resp.physical_base \
        + kern_resp.virtual_base)

void refresh_pages(pml4_t pml4)
{
    __asm__ (
        "mov %0, %%cr3\n"
        :
        : "r" (K2PHYSK(pml4 ? pml4 : kpml4))
        : "memory"
    );
    cur_page_tab = pml4;
}

/* 
 * Maps the pages from src to src+length to the virtual addresses of dst to
 * dst+length, with flags as specified by the flags parameter.
 *
 * dst is the target virtual address, and src is the source physical address.
 * They will be rounded down to the nearest page.
 * Length will be rounded up.
 */
void premap_pages(
        uintptr_t dst,
        uintptr_t src,
        uintptr_t length,
        int flags
    )
{
    uintptr_t dst_end = (dst + length + 0xFFF) & ~0xFFF;
    uintptr_t src_end = (src + length + 0xFFF) & ~0xFFF;

    while (dst < dst_end && src < src_end) {
        int pml4_idx = (dst >> 39) & 0x1FF;
        int pdpt_idx = (dst >> 30) & 0x1FF;
        int pd_idx   = (dst >> 21) & 0x1FF;
        int pt_idx   = (dst >> 12) & 0x1FF;

        if (!(kpml4[pml4_idx] & PAGE_PRESENT))
            kpml4[pml4_idx] = (uintptr_t) K2PHYSK(miniheap_alloc(PAGE_SIZE))
                | PAGE_PRESENT | PAGE_WRITABLE;

        pdpt_t pdpt = PHYSK2VK(kpml4[pml4_idx] & ~0xFFF);
        if (!(pdpt[pdpt_idx] & PAGE_PRESENT))
            pdpt[pdpt_idx] = (uintptr_t) K2PHYSK(miniheap_alloc(PAGE_SIZE)) |
                PAGE_PRESENT | PAGE_WRITABLE;

        pd_t pd = PHYSK2VK(pdpt[pdpt_idx] & ~0xFFF);
        if (!(pd[pd_idx] & PAGE_PRESENT))
            pd[pd_idx] = (uintptr_t) K2PHYSK(miniheap_alloc(PAGE_SIZE)) |
                PAGE_PRESENT | PAGE_WRITABLE;

        pt_t pt = (pt_t) PHYSK2VK(pd[pd_idx] & ~0xFFF);
        pt[pt_idx] = (src & ~0xFFF) | flags;

        dst += PAGE_SIZE;
        src += PAGE_SIZE;
    }
}


void map_pages(
        pml4_t pml4,
        uintptr_t dst,
        uintptr_t src,
        uintptr_t length,
        int flags
    )
{
    uintptr_t dst_end = (dst + length + 0xFFF) & ~0xFFF;
    uintptr_t src_end = (src + length + 0xFFF) & ~0xFFF;

    while (dst < dst_end && src < src_end) {
        int pml4_idx = (dst >> 39) & 0x1FF;
        int pdpt_idx = (dst >> 30) & 0x1FF;
        int pd_idx   = (dst >> 21) & 0x1FF;
        int pt_idx   = (dst >> 12) & 0x1FF;

        if (!(pml4[pml4_idx] & PAGE_PRESENT))
            pml4[pml4_idx] = (uintptr_t) K2PHYSK(miniheap_alloc(PAGE_SIZE))
                | flags;

        pdpt_t pdpt = PHYSK2VK(pml4[pml4_idx] & ~0xFFF);
        if (!(pdpt[pdpt_idx] & PAGE_PRESENT))
            pdpt[pdpt_idx] = (uintptr_t) K2PHYSK(miniheap_alloc(PAGE_SIZE)) |
                flags;

        pd_t pd = PHYSK2VK(pdpt[pdpt_idx] & ~0xFFF);
        if (!(pd[pd_idx] & PAGE_PRESENT))
            pd[pd_idx] = (uintptr_t) K2PHYSK(miniheap_alloc(PAGE_SIZE)) |
                flags;

        pt_t pt = (pt_t) PHYSK2VK(pd[pd_idx] & ~0xFFF);
        pt[pt_idx] = (src & ~0xFFF) | flags;

        dst += PAGE_SIZE;
        src += PAGE_SIZE;
    }
}

void map_page_default(uintptr_t dst, uintptr_t src, uintptr_t length)
{
    premap_pages(dst, src, length, PAGE_PRESENT | PAGE_WRITABLE);
}


uintptr_t end_pos;

void map_kern_pages(void)
{
    if (kern_addr_req.response == NULL)
        panic("Could not get kernel physical addr");

    kern_resp = *kern_addr_req.response;

    premap_pages(
        (uintptr_t) kern_load,
        (uintptr_t) K2PHYSK(kern_load),
        kern_end - kern_load,
        PAGE_PRESENT | PAGE_WRITABLE
    );
    end_pos = round_up_page((uintptr_t) kern_end);
}


void map_screen(void)
{
    size_t len =  screen.pitch * (screen.bpp/8) * screen.height;
    premap_pages(
        (uintptr_t) end_pos,
        (uintptr_t) screen.paddr,
        len,
        PAGE_PRESENT | PAGE_WRITABLE
    );
    screen.vaddr = (void *) end_pos;
    end_pos = round_up_page((uintptr_t) end_pos + len);
}

void *kmap_phys(void *phys, size_t len)
{
    uintptr_t aligned = (uintptr_t) phys & ~0xFFF;
    uintptr_t res = end_pos + ((uintptr_t) phys - aligned);

    map_pages(
        kpml4,
        (uintptr_t) end_pos,
        aligned,
        len,
        PAGE_PRESENT | PAGE_WRITABLE
    );
    end_pos = round_up_page(end_pos + len);

    for (uintptr_t p = res & ~0xFFF; p < end_pos; p += PAGE_SIZE)
        __asm__ volatile ("invlpg (%0)" : : "b" (p) : "memory" );

    return (void *) res;
}

void kunmap(void *virt, size_t len)
{
    map_pages(kpml4, (uintptr_t) virt, 0, len, 0);
    for (void *p = virt; p < (void *) virt + len; p += PAGE_SIZE)
        __asm__ volatile ("invlpg (%0)" : : "b" (p) : "memory" );
}

void user_main();
pml4_t newproc_pages(void)
{
    pml4_t new_pml4 = miniheap_alloc(PAGE_SIZE);
    memset(new_pml4, 0, PAGE_SIZE);

    map_pages(
        new_pml4,
        (uintptr_t) kern_load,
        (uintptr_t) K2PHYSK(kern_load),
        kern_end - kern_load,
        PAGE_PRESENT | PAGE_WRITABLE
    );

    map_pages(
        new_pml4,
        (uintptr_t) 0x100000,
        (uintptr_t) K2PHYSK(user_main),
        0x1000,
        PAGE_PRESENT | PAGE_USER | PAGE_WRITABLE
    );

    return new_pml4;
}
