#include <stdint.h>
#include <mem.h>
#include <kern.h>

#define ACCESS_PRESENT  (1 << 7)

#define ACCESS_S        (1 << 4) /* TSS if 0 */
#define ACCESS_EXEC     (1 << 3)
#define ACCESS_DC       (1 << 2)
#define ACCESS_RW       (1 << 1)
#define ACCESS_ACCESSED (1 << 0)


#define GRAN_4K   (1 << 7)
#define SZ_32     (1 << 6)
#define LONG_MODE (1 << 5)

typedef struct gdt_desc
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_desc;

struct gdtptr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdtr;

typedef struct tss_t
{
    uint32_t reserved1;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved2;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved3;
    uint16_t reserved4;
    uint16_t iopb;
} __attribute__((packed)) tss_t;

typedef struct tss_desc
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
    uint32_t base_upper;
    uint32_t reserved;
} __attribute__((packed)) tss_desc;

tss_t kernel_tss;

static struct gdt_desc gdt_descs[] = {
    {0},
    { /* Kernel Code */
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_mid = 0,
        .access = ACCESS_PRESENT | ACCESS_S | ACCESS_EXEC | ACCESS_RW,
        .granularity = GRAN_4K | LONG_MODE | 0xF,
        .base_high = 0
    },
    { /* Kernel Data */
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_mid = 0,
        .access = ACCESS_PRESENT | ACCESS_S | ACCESS_RW,
        .granularity = GRAN_4K | SZ_32 | 0xF,
        .base_high = 0
    },
    { /* User Code */
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_mid = 0,
        .access = ACCESS_PRESENT | ACCESS_S | ACCESS_EXEC | ACCESS_RW | 3 << 5,
        .granularity = GRAN_4K | LONG_MODE | 0xF,
        .base_high = 0
    },
    { /* User Data */
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_mid = 0,
        .access = ACCESS_PRESENT | ACCESS_S | ACCESS_RW | 3 << 5,
        .granularity = GRAN_4K | SZ_32 | 0xF,
        .base_high = 0
    },
    {0}, /* TSS Segment */
    {0} /* TSS Descriptors are larger than normal GDT descriptors */
};

char tss_stack[4096] __attribute__((aligned(16)));

void create_tss_desc(tss_desc *desc, uint64_t base) {
    int limit = sizeof(tss_t) - 1;
    desc->limit_low = limit & 0xFFFF;
    desc->base_low = base & 0xFFFF;
    desc->base_middle = (base >> 16) & 0xFF;
    desc->access = 0x89;
    desc->granularity = (limit >> 16) & 0x0F;
    desc->base_high = (base >> 24) & 0xFF;
    desc->base_upper = (base >> 32) & 0xFFFFFFFF;
    desc->reserved = 0;
}

void init_tss(void)
{
    kernel_tss.iopb = 0xFFFF;
    /* iopb is disabled by having a value larger than limit
     * This disables I/O access */
    kernel_tss.rsp0 = (uint64_t) tss_stack + sizeof(tss_stack);
    create_tss_desc((tss_desc *) &gdt_descs[5], (uint64_t) &kernel_tss);
}

void flush_gdt(void);
void gdt_init(void)
{
    init_tss();
    gdtr.limit = sizeof(gdt_desc)*5 + sizeof(tss_desc) - 1;
    gdtr.base = (uintptr_t) &gdt_descs;

    flush_gdt();
}
