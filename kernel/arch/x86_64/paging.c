#include <u.h>
#include <lock.h>
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
#include <map.h>
#include <arch/addr.h>
#include <mp.h>

struct limine_kernel_address_request kern_addr_req = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST
};

static struct limine_kernel_address_response kern_resp;

#define X86_PAGE_PRESENT  (1 << 0)
#define X86_PAGE_WRITABLE (1 << 1)
#define X86_PAGE_USER     (1 << 2)
#define X86_PAGE_NOCACHE  (1 << 4)
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

static __attribute__((aligned(PAGE_SIZE))) pml4e_t kpml4[512];
struct addrspace kspace = { .addr = kpml4 };

extern char kern_load[], kern_end[],
        text_start[], text_end[],
        rodata_start[], rodata_end[],
        data_start[], data_end[], page_tmp[];

static uintptr_t end_pos;
static lock_t end_pos_lock = LOCK_INIT;

#define VK2PHYSK(x) ( \
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
        uint64_t flags
    )
{
    int x86_flags =
        (!!(flags & PAGE_PRESENT) * X86_PAGE_PRESENT)
        | (!!(flags & PAGE_USER) * X86_PAGE_USER)
        | (!!(flags & PAGE_WRITABLE) * X86_PAGE_WRITABLE)
        | (!!(flags & PAGE_NX) * X86_PAGE_NX)
        | (!!(flags & PAGE_NOCACHE) * X86_PAGE_NOCACHE);
    int top_flags = X86_PAGE_PRESENT | X86_PAGE_USER | X86_PAGE_WRITABLE;
    uintptr_t dst_end = (dst + length + 0xFFF) & ~0xFFF;
    uintptr_t src_end = (src + length + 0xFFF) & ~0xFFF;

    while (dst < dst_end && src < src_end)
    {
        int pml4_idx = (dst >> 39) & 0x1FF;
        int pdpt_idx = (dst >> 30) & 0x1FF;
        int pd_idx   = (dst >> 21) & 0x1FF;
        int pt_idx   = (dst >> 12) & 0x1FF;

        if (!(kpml4[pml4_idx] & X86_PAGE_PRESENT))
            kpml4[pml4_idx] = VK2PHYSK(miniheap_alloc(PAGE_SIZE)) | top_flags;

        pdpt_t pdpt = PHYSK2VK(kpml4[pml4_idx] & ADDR_MASK);
        if (!(pdpt[pdpt_idx] & X86_PAGE_PRESENT))
            pdpt[pdpt_idx] = VK2PHYSK(miniheap_alloc(PAGE_SIZE)) | top_flags;

        pd_t pd = PHYSK2VK(pdpt[pdpt_idx] & ADDR_MASK);
        if (!(pd[pd_idx] & X86_PAGE_PRESENT))
            pd[pd_idx] = VK2PHYSK(miniheap_alloc(PAGE_SIZE)) | top_flags;

        pt_t pt = PHYSK2VK(pd[pd_idx] & ADDR_MASK);
        pt[pt_idx] = (src & ADDR_MASK) | x86_flags;

        dst += PAGE_SIZE;
        src += PAGE_SIZE;
    }
}

static void *map_page_tmp(uintptr_t phys)
{
    premap_pages(
        (uintptr_t) page_tmp,
        phys,
        PAGE_SIZE,
        PAGE_PRESENT | PAGE_WRITABLE
    );
    __asm__ volatile ("invlpg (%0)" : : "b" (page_tmp) : "memory" );
    return page_tmp;
}

uintptr_t kvirt2phys(struct addrspace *cur_space, void *virt)
{
    int pml4_idx = ((uintptr_t) virt >> 39) & 0x1FF;
    int pdpt_idx = ((uintptr_t) virt >> 30) & 0x1FF;
    int pd_idx   = ((uintptr_t) virt >> 21) & 0x1FF;
    int pt_idx   = ((uintptr_t) virt >> 12) & 0x1FF;

    pml4_t pml4 = cur_space ? cur_space->addr : kpml4;

    assert(virt != 0);

    assert(pml4[pml4_idx] & X86_PAGE_PRESENT);
    pdpt_t pdpt = map_page_tmp(pml4[pml4_idx] & ADDR_MASK);

    assert(pdpt[pdpt_idx] & X86_PAGE_PRESENT);
    pd_t pd = map_page_tmp(pdpt[pdpt_idx] & ADDR_MASK);

    assert(pd[pd_idx] & X86_PAGE_PRESENT);
    pt_t pt = map_page_tmp(pd[pd_idx] & ADDR_MASK);

    return (pt[pt_idx] & ADDR_MASK) | ((uintptr_t) virt & 0xFFF);
}

void flush_pages(struct addrspace *space)
{
    uintptr_t phys;
    struct cpu_data *cpu = get_cpu_data();

    if (space == NULL) phys = VK2PHYSK(kpml4);
    else
    {
        phys = kvirt2phys(cpu->proc_current->space, space->addr);
        cpu->proc_current->space = space;
    }

    __asm__ (
        "mov %0, %%cr3\n"
        :
        : "r" (phys)
        : "memory"
    );
}

void map_kern_pages(void)
{
    if (kern_addr_req.response == NULL)
        panic("Could not get kernel physical addr");

    kern_resp = *kern_addr_req.response;

    ACQUIRE(&end_pos_lock);

    premap_pages(
        (uintptr_t) text_start,
        VK2PHYSK(text_start),
        text_end - text_start,
        PAGE_PRESENT
    );
    premap_pages(
        (uintptr_t) rodata_start,
        VK2PHYSK(rodata_start),
        rodata_end - rodata_start,
        PAGE_PRESENT | PAGE_NX
    );
    premap_pages(
        (uintptr_t) data_start,
        VK2PHYSK(data_start),
        data_end - data_start,
        PAGE_PRESENT | PAGE_NX | PAGE_WRITABLE
    );
    end_pos = round_up_page((uintptr_t) kern_end);
    RELEASE(&end_pos_lock);
}

void map_screen(void)
{
    size_t len =  screen.pitch * (screen.bpp/8) * screen.height;
    ACQUIRE(&end_pos_lock);
    premap_pages(
        (uintptr_t) end_pos,
        (uintptr_t) screen.paddr,
        len,
        PAGE_PRESENT | PAGE_WRITABLE | PAGE_NX
    );
    screen.vaddr = end_pos;
    end_pos = round_up_page((uintptr_t) end_pos + len);
    RELEASE(&end_pos_lock);
}

void refresh_pages(void *start, void *end)
{
    for (
        uintptr_t p = (uintptr_t) start & ~0xFFF;
        p < (uintptr_t) end;
        p += PAGE_SIZE
    )
        __asm__ volatile ("invlpg (%0)" : : "b" (p) : "memory" );
}

void *premap_anon(uintptr_t phys, uintptr_t len, int flags)
{
    uintptr_t aligned = phys & ~0xFFF;
    uintptr_t offset = phys - aligned;
    void *res;
    ACQUIRE(&end_pos_lock);
    res = (void *) (end_pos + offset);

    premap_pages(
        end_pos,
        aligned,
        len,
        flags
    );

    end_pos = round_up_page(end_pos + len);

    refresh_pages(res, (void *) end_pos);

    RELEASE(&end_pos_lock);
    return (void *) res;
}

void unmap_pages(void *virt, size_t len)
{
    premap_pages((uintptr_t) virt, 0, len, 0);
    refresh_pages(virt, (void *) round_up_page((uintptr_t) virt + len));
}

static uintptr_t tab_new(struct addrspace *space)
{
    struct cpu_data *cpu = get_cpu_data();
    struct addrspace *cur_space = cpu->proc_current
        ? cpu->proc_current->space
        : NULL;

    void *virt = page_alloc(PAGE_SIZE);
    uintptr_t phys = kvirt2phys(cur_space, virt);
    map_insert(&space->physmap, &phys, sizeof(phys), &virt, sizeof(virt));
    return phys;
}

static void *tab_get(struct addrspace *space, uintptr_t phys)
{
    void **res = map_get(space->physmap, &phys, sizeof(phys));
    if (res == NULL && space == &kspace) return PHYSK2VK(phys);
    if (res == NULL)
    {
        panic("Could not find in map: %p", phys);
    }
    return *res;
}

void map_pages(
        struct addrspace *space,
        uintptr_t dst,
        uintptr_t src,
        uintptr_t length,
        uint64_t flags
    )
{
    int x86_flags =
        (!!(flags & PAGE_PRESENT) * X86_PAGE_PRESENT)
        | (!!(flags & PAGE_USER) * X86_PAGE_USER)
        | (!!(flags & PAGE_WRITABLE) * X86_PAGE_WRITABLE)
        | (!!(flags & PAGE_NX) * X86_PAGE_NX)
        | (!!(flags & PAGE_NOCACHE) * X86_PAGE_NOCACHE);
    int top_flags = X86_PAGE_PRESENT | X86_PAGE_USER | X86_PAGE_WRITABLE;
    uintptr_t dst_end = (dst + length + 0xFFF) & ~0xFFF;
    uintptr_t src_end = (src + length + 0xFFF) & ~0xFFF;
    pml4_t pml4 = space->addr;

    while (dst < dst_end && src < src_end)
    {
        int pml4_idx = (dst >> 39) & 0x1FF;
        int pdpt_idx = (dst >> 30) & 0x1FF;
        int pd_idx   = (dst >> 21) & 0x1FF;
        int pt_idx   = (dst >> 12) & 0x1FF;

        if (!(pml4[pml4_idx] & X86_PAGE_PRESENT))
            pml4[pml4_idx] = tab_new(space) | top_flags;

        pdpt_t pdpt = tab_get(space, pml4[pml4_idx] & ADDR_MASK);
        if (!(pdpt[pdpt_idx] & X86_PAGE_PRESENT))
            pdpt[pdpt_idx] = tab_new(space) | top_flags;

        pd_t pd = tab_get(space, pdpt[pdpt_idx] & ADDR_MASK);
        if (!(pd[pd_idx] & X86_PAGE_PRESENT))
            pd[pd_idx] = tab_new(space) | top_flags;

        pt_t pt = tab_get(space, pd[pd_idx] & ADDR_MASK);
        pt[pt_idx] = (src & ADDR_MASK) | x86_flags;

        dst += PAGE_SIZE;
        src += PAGE_SIZE;
    }
}

void *map_anon(uintptr_t phys, uintptr_t len, int flags)
{
    uintptr_t aligned = phys & ~0xFFF;
    uintptr_t offset = phys - aligned;
    struct cpu_data *cpu = get_cpu_data();
    struct addrspace *space = cpu->proc_current ? cpu->proc_current->space : &kspace;
    void *res;
    ACQUIRE(&end_pos_lock);

    res = (void *) (end_pos + offset);

    map_pages(
        space,
        end_pos,
        aligned,
        len,
        flags
    );

    end_pos = round_up_page(end_pos + len);

    refresh_pages(res, (void *) end_pos);

    RELEASE(&end_pos_lock);
    return (void *) res;
}


void addrspace_free(void *addr)
{
    page_free(addr, PAGE_SIZE);
}

void proc_pages_init(struct proc *p, void *start, size_t len)
{
    p->space = kalloc(sizeof(struct addrspace));
    p->space->physmap = map_new(0x10);
    p->space->addr = page_alloc(PAGE_SIZE);

    ((pml4_t) p->space->addr)[511] = kpml4[511];

    p->segs = FlexAlloc(struct segment, 2);

    p->segs->item[0].base = (void *) PROC_ENTRY;
    p->segs->item[0].kvirt = start;
    p->segs->item[0].len = len;

    map_pages(
        p->space,
        (uintptr_t) p->segs->item[0].base,
        (uintptr_t) p->segs->item[0].kvirt,
        p->segs->item[0].len,
        PAGE_PRESENT | PAGE_USER
    );

    p->segs->item[1].base = (void *) STACK_TOP;
    p->segs->item[1].len = 64*PAGE_SIZE;
    p->segs->item[1].kvirt = page_alloc(p->segs->item[1].len);

    map_pages(
        p->space,
        (uintptr_t) p->segs->item[1].base,
        (uintptr_t) p->segs->item[1].kvirt,
        p->segs->item[1].len,
        PAGE_PRESENT | PAGE_USER | PAGE_NX | PAGE_WRITABLE
    );
}

void mp_pages(void)
{
    flush_pages(NULL);
}
