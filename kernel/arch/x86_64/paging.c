#include <u.h>
#include <assert.h>
#include <stdint.h>
#include <paging.h>
#include <mem.h>
#include <limine.h>
#include <screen.h>
#include <panic.h>
#include <miniheap.h>
#include <page_alloc.h>
#include <proc.h>
#include "addr.h"

struct limine_kernel_address_request kern_addr_req = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST
};

static struct limine_kernel_address_response kern_resp;

page_tab cur_page_tab;

#define X86_PAGE_PRESENT  (1 << 0)
#define X86_PAGE_WRITABLE (1 << 1)
#define X86_PAGE_USER     (1 << 2)
#define X86_PAGE_LARGE    (1 << 7)
#define X86_PAGE_NX       (1ULL << 63)

#define ADDR_MASK (~(0xFFF | (1ULL << 63)))

typedef uintptr_t pml4e_t;
typedef uintptr_t pdpte_t;
typedef uintptr_t pde_t;
typedef uintptr_t pte_t;

typedef pml4e_t *pml4_t;
typedef pdpte_t *pdpt_t;
typedef pde_t   *pd_t;
typedef pte_t   *pt_t;

pml4e_t kpml4[512] __attribute__((aligned(PAGE_SIZE)));

extern char kern_load[], kern_end[], page_tmp[];
uintptr_t end_pos;

uint8_t tmp_pages[2][PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

#define K2PHYSK(x) ( \
        (uintptr_t)(x) - kern_resp.virtual_base \
        + kern_resp.physical_base)

#define PHYSK2VK(x) (void *)( \
        (uintptr_t)(x) - kern_resp.physical_base \
        + kern_resp.virtual_base)

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
        uint64_t x86_flags
    )
{
    const int top_flags = X86_PAGE_PRESENT | X86_PAGE_USER | X86_PAGE_WRITABLE;
    uintptr_t dst_end = (dst + length + 0xFFF) & ~0xFFF;
    uintptr_t src_end = (src + length + 0xFFF) & ~0xFFF;

    while (dst < dst_end && src < src_end) {
        int pml4_idx = (dst >> 39) & 0x1FF;
        int pdpt_idx = (dst >> 30) & 0x1FF;
        int pd_idx   = (dst >> 21) & 0x1FF;
        int pt_idx   = (dst >> 12) & 0x1FF;

        if (!(kpml4[pml4_idx] & X86_PAGE_PRESENT))
            kpml4[pml4_idx] = K2PHYSK(miniheap_alloc(PAGE_SIZE)) | top_flags;

        pdpt_t pdpt = PHYSK2VK(kpml4[pml4_idx] & ADDR_MASK);
        if (!(pdpt[pdpt_idx] & X86_PAGE_PRESENT))
            pdpt[pdpt_idx] = K2PHYSK(miniheap_alloc(PAGE_SIZE)) | top_flags;

        pd_t pd = PHYSK2VK(pdpt[pdpt_idx] & ADDR_MASK);
        if (!(pd[pd_idx] & X86_PAGE_PRESENT))
            pd[pd_idx] = K2PHYSK(miniheap_alloc(PAGE_SIZE)) | top_flags;

        pt_t pt = PHYSK2VK(pd[pd_idx] & ADDR_MASK);
        pt[pt_idx] = (src & ADDR_MASK) | x86_flags;

        dst += PAGE_SIZE;
        src += PAGE_SIZE;
    }
}

void *map_page_tmp(void *dst, uintptr_t phys)
{
    premap_pages(
        (uintptr_t) dst,
        phys,
        PAGE_SIZE,
        X86_PAGE_PRESENT | X86_PAGE_WRITABLE
    );
    __asm__ volatile ("invlpg (%0)" : : "b" (dst) : "memory" );
    return dst;
}

uintptr_t kvirt2phys(page_tab pml4, void *virt)
{
    int pml4_idx = ((uintptr_t) virt >> 39) & 0x1FF;
    int pdpt_idx = ((uintptr_t) virt >> 30) & 0x1FF;
    int pd_idx   = ((uintptr_t) virt >> 21) & 0x1FF;
    int pt_idx   = ((uintptr_t) virt >> 12) & 0x1FF;

    assert(virt != 0);

    assert(pml4[pml4_idx] & X86_PAGE_PRESENT);
    pdpt_t pdpt = map_page_tmp(tmp_pages[1], pml4[pml4_idx] & ADDR_MASK);

    assert(pdpt[pdpt_idx] & X86_PAGE_PRESENT);
    pd_t pd = map_page_tmp(tmp_pages[1], pdpt[pdpt_idx] & ADDR_MASK);

    assert(pd[pd_idx] & X86_PAGE_PRESENT);
    pt_t pt = map_page_tmp(tmp_pages[1], pd[pd_idx] & ADDR_MASK);

    return (pt[pt_idx] & ADDR_MASK) | ((uintptr_t) virt & 0xFFF);
}

void refresh_pages(pml4_t pml4)
{
    if (pml4 == NULL) pml4 = kpml4;
    __asm__ (
        "mov %0, %%cr3\n"
        :
        : "r" (cur_page_tab ? kvirt2phys(cur_page_tab, pml4) : K2PHYSK(pml4))
        : "memory"
    );
    cur_page_tab = pml4;
}

void map_kern_pages(void)
{
    if (kern_addr_req.response == NULL)
        panic("Could not get kernel physical addr");

    kern_resp = *kern_addr_req.response;

    premap_pages(
        (uintptr_t) kern_load,
        K2PHYSK(kern_load),
        kern_end - kern_load,
        X86_PAGE_PRESENT | X86_PAGE_WRITABLE
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
        X86_PAGE_PRESENT | X86_PAGE_WRITABLE | X86_PAGE_NX
    );
    screen.vaddr = (void *) end_pos;
    end_pos = round_up_page((uintptr_t) end_pos + len);
}

void *kmap_phys(void *phys, size_t len)
{
    uintptr_t aligned = (uintptr_t) phys & ~0xFFF;
    uintptr_t res = end_pos + ((uintptr_t) phys - aligned);

    premap_pages(
        (uintptr_t) end_pos,
        aligned,
        len,
        X86_PAGE_PRESENT | X86_PAGE_WRITABLE
    );
    end_pos = round_up_page(end_pos + len);

    for (uintptr_t p = res & ~0xFFF; p < end_pos; p += PAGE_SIZE)
        __asm__ volatile ("invlpg (%0)" : : "b" (p) : "memory" );

    return (void *) res;
}

void kunmap(void *virt, size_t len)
{
    premap_pages((uintptr_t) virt, 0, len, 0);
    for (void *p = virt; p < (void *) virt + len; p += PAGE_SIZE)
        __asm__ volatile ("invlpg (%0)" : : "b" (p) : "memory" );
}

uintptr_t new_tab(struct proc *p)
{
    void *virtpage = page_alloc(PAGE_SIZE);
    uintptr_t physpage = kvirt2phys(kpml4, virtpage); 
    map_set(&p->addrs, &physpage, sizeof(physpage), &virtpage, sizeof(virtpage));
    return physpage;
}

void map_user_pages(
        struct proc *p,
        uintptr_t dst,
        uintptr_t src,
        size_t length,
        int flags
    )
{
    int x86_flags =
        (!!(flags & PAGE_PRESENT) * X86_PAGE_PRESENT)
        | (!!(flags & PAGE_USER) * X86_PAGE_USER)
        | (!!(flags & PAGE_WRITABLE) * X86_PAGE_WRITABLE)
        | (!!(flags & PAGE_NX) * X86_PAGE_NX);
    const int top_flags = X86_PAGE_PRESENT | X86_PAGE_USER | X86_PAGE_WRITABLE;
    uintptr_t dst_end = (dst + length + 0xFFF) & ~0xFFF;
    uintptr_t src_end = (src + length + 0xFFF) & ~0xFFF;

    while (dst < dst_end && src < src_end) {
        uintptr_t addr;
        int pml4_idx = (dst >> 39) & 0x1FF;
        int pdpt_idx = (dst >> 30) & 0x1FF;
        int pd_idx   = (dst >> 21) & 0x1FF;
        int pt_idx   = (dst >> 12) & 0x1FF;

        if (!(p->pt[pml4_idx] & X86_PAGE_PRESENT))
            p->pt[pml4_idx] = new_tab(p) | top_flags;
    
        addr = p->pt[pml4_idx] & ADDR_MASK;
        pdpt_t pdpt = *(pdpt_t *)map_get(p->addrs, &addr, sizeof(addr));
        if (!(pdpt[pdpt_idx] & X86_PAGE_PRESENT))
            pdpt[pdpt_idx] = new_tab(p) | top_flags;

        addr = pdpt[pdpt_idx] & ADDR_MASK;
        pd_t pd = *(pd_t *)map_get(p->addrs, &addr, sizeof(addr));
        if (!(pd[pd_idx] & X86_PAGE_PRESENT))
            pd[pd_idx] = new_tab(p) | top_flags;

        addr = pd[pd_idx] & ADDR_MASK;
        pt_t pt = *(pt_t *)map_get(p->addrs, &addr, sizeof(addr));
        pt[pt_idx] = (src & ADDR_MASK) | x86_flags;

        dst += PAGE_SIZE;
        src += PAGE_SIZE;
    }
}

void user_main(void);
void newproc_pages(void *pv)
{
    struct proc *p = pv;
    p->addrs = map_new(16);
    p->pt = page_alloc(PAGE_SIZE);

    p->pt[511] = kpml4[511];

    map_user_pages(
        p,
        0x100000,
        (uintptr_t)
        K2PHYSK(user_main),
        PAGE_SIZE,
        PAGE_PRESENT | PAGE_USER | PAGE_WRITABLE
    );
}
