#include <stdint.h>
#include <phys_malloc.h>
#include <mem.h>

#define PAGE_PRESENT  1
#define PAGE_WRITABLE (1 << 1)
#define PAGE_USER     (1 << 2)
#define PAGE_LARGE    (1 << 7)
#define PAGE_NOEXEC   (1ULL << 63)

typedef uintptr_t pml4e_t;
typedef uintptr_t pdpte_t;
typedef uintptr_t pde_t;
typedef uintptr_t pte_t;

typedef pml4e_t pml4_t[512] __attribute__((aligned(0x1000)));
typedef pdpte_t pdpt_t[512] __attribute__((aligned(0x1000)));
typedef pde_t   pd_t[512]   __attribute__((aligned(0x1000)));
typedef pte_t   pt_t[512]   __attribute__((aligned(0x1000)));

pml4_t pml4;

void *kern_load, *kern_end;
#define K2PHYS(x) (void *)((uintptr_t)(x) - (uintptr_t) &kern_load)

void map_kern_pages(void)
{
    int i;
    uintptr_t vaddr;
    for (vaddr = (uintptr_t) &kern_load;
            vaddr < (uintptr_t) &kern_end;
            vaddr += 0x1000
        )
    {
        uintptr_t phys = (uintptr_t) K2PHYS(vaddr);
        
        int pml4_idx = (vaddr >> 39) & 0x1FF;
        int pdpt_idx = (vaddr >> 30) & 0x1FF;
        int pd_idx   = (vaddr >> 21) & 0x1FF;
        int pt_idx   = (vaddr >> 12) & 0x1FF;

        if (!(pml4[pml4_idx] & PAGE_PRESENT))
            pml4[pml4_idx] = (uintptr_t) phys_valloc(0x1000) |
                    PAGE_PRESENT | PAGE_WRITABLE;

        pdpt_t *pdpt = (pdpt_t *)(pml4[pml4_idx] & ~0xFFF);
        if (!((*pdpt)[pdpt_idx] & PAGE_PRESENT))
            (*pdpt)[pdpt_idx] = (uintptr_t) phys_valloc(0x1000) |
                    PAGE_PRESENT | PAGE_WRITABLE;

        pd_t *pd = (pd_t *)((*pdpt)[pdpt_idx] & ~0xFFF);
        if (!((*pd)[pd_idx] & PAGE_PRESENT))
            (*pd)[pd_idx] = (uintptr_t) phys_valloc(0x1000) |
                    PAGE_PRESENT | PAGE_WRITABLE;

        pt_t *pt = (pt_t *)((*pd)[pd_idx] & ~0xFFF);
        (*pt)[pt_idx] = (phys & ~0xFFF) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    __asm__ (
            "mov %0, %%cr3\n"
            :
            : "r" ((uintptr_t) pml4)
            : "memory"
        );
}
