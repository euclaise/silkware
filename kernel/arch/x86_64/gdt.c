#include <stdint.h>
#include <mem.h>
#include <kern.h>
#include <util.h>

#define ACCESS_PRESENT  (1 << 7)

#define ACCESS_S        (1 << 4) /* TSS if 0 */
#define ACCESS_EXEC     (1 << 3)
#define ACCESS_DC       (1 << 2)
#define ACCESS_RW       (1 << 1)
#define ACCESS_ACCESSED (1 << 0)


#define GRAN_4K   (1 << 7)
#define SZ_32     (1 << 6)
#define LONG_MODE (1 << 5)

struct gdt_desc
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} _packed;

struct gdtptr
{
    uint16_t limit;
    uint64_t base;
} _packed gdtr;

struct tss
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
} _packed;

struct tss_desc
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
    uint32_t base_upper;
    uint32_t reserved;
} _packed;

struct tss kernel_tss;


struct gdt
{
    struct gdt_desc entries[5];
    struct tss_desc tss;
} _packed gdt = {
    .entries = {
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
        }
    }
};

char tss_stack[4096] __attribute__((aligned(16)));

void init_tss(void)
{
    int limit = sizeof(struct tss) - 1;
    uint64_t base = (uint64_t) &kernel_tss;

    /* iopb is disabled by having a value larger than limit
     * This disables I/O access */
    kernel_tss.iopb = 0xFFFF;
    kernel_tss.rsp0 = (uint64_t) tss_stack + sizeof(tss_stack);

    gdt.tss.limit_low = limit & 0xFFFF;
    gdt.tss.base_low = base & 0xFFFF;
    gdt.tss.base_middle = (base >> 16) & 0xFF;
    gdt.tss.access = 0x89;
    gdt.tss.granularity = (limit >> 16) & 0x0F;
    gdt.tss.base_high = (base >> 24) & 0xFF;
    gdt.tss.base_upper = (base >> 32) & 0xFFFFFFFF;
    gdt.tss.reserved = 0;
}

void flush_gdt(void);
void gdt_init(void)
{
    init_tss();

    gdtr.limit = sizeof(struct gdt_desc)*5 + sizeof(struct tss_desc) - 1;
    gdtr.base = (uintptr_t) &gdt;

    flush_gdt();
}
