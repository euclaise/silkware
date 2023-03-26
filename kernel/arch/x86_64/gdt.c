#include <stdint.h>
#include <mem.h>
#include <kern.h>

#define ACCESS_PRESENT  (1 << 7)
#define ACCESS_DPL      (1 << 5)
#define ACCESS_NOTSYS   (1 << 4)
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
    uint16_t iomap_base;
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
    { /* Code */
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_mid = 0,
        .access = ACCESS_PRESENT | ACCESS_EXEC | ACCESS_RW,
        .granularity = GRAN_4K | LONG_MODE | 0xF,
        .base_high = 0
    },
    { /* Data */
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_mid = 0,
        .access = ACCESS_PRESENT | ACCESS_RW,
        .granularity = GRAN_4K | SZ_32 | 0xF,
        .base_high = 0
    },
    {0}, /* TSS Segment */
    {0} /* TSS Descriptors are larger than normal GDT descriptors */
};

char tss_stack[4096] __attribute__((aligned(16)));
tss_desc create_tss_desc(uint64_t base) {
    tss_desc desc;
    desc.limit_low = sizeof(tss_t) & 0xFFFF;
    desc.base_low = base & 0xFFFF;
    desc.base_middle = (base >> 16) & 0xFF;
    desc.access = 0x89;
    desc.granularity = ((sizeof(tss_t) >> 16) & 0x0F);
    desc.base_high = (base >> 24) & 0xFF;
    desc.base_upper = (base >> 32) & 0xFFFFFFFF;
    desc.reserved = 0;
    return desc;
}

void init_tss(void)
{
    kernel_tss.iomap_base = 0xFFFF;
    kernel_tss.rsp0 = (uint64_t) tss_stack + sizeof(tss_stack);

    tss_desc desc = create_tss_desc((uint64_t) &kernel_tss);
    memcpy(
        &gdt_descs[3],
        &desc,
        sizeof(tss_desc)
    );
}

void flush_gdt(void);
void setup_gdt(void)
{
    init_tss();
    gdtr.limit = sizeof(gdt_descs[0])*3 + sizeof(tss_desc) - 1;
    gdtr.base = (uintptr_t) &gdt_descs;

    flush_gdt();
}
