#include <types.h>
#include <mem.h>
#include <kern.h>
#include <util.h>
#include <assert.h>
#include <mp.h>
#include <page_alloc.h>

#define ACCESS_PRESENT  (1 << 7)
#define ACCESS_S        (1 << 4) /* TSS if 0 */
#define ACCESS_EXEC     (1 << 3)
#define ACCESS_DC       (1 << 2) /* Direction bit / conforming bit, keep off */
#define ACCESS_RW       (1 << 1)
#define ACCESS_ACCESSED (1 << 0)

#define DPL(x) ((x) << 5) /* Privilege level */

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

struct gdtr
{
    uint16_t limit;
    uint64_t base;
} _packed;

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

struct tss *cpu_tss;

struct gdt
{
    struct gdt_desc entries[5];
    struct tss_desc tss;
} _packed gdt_base = {
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
        { /* User Data */
            .limit_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .access = ACCESS_PRESENT | ACCESS_S | ACCESS_RW | DPL(3),
            .granularity = GRAN_4K | SZ_32 | 0xF,
            .base_high = 0
        },
        { /* User Code */
            .limit_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .access = ACCESS_PRESENT | ACCESS_S | ACCESS_EXEC | ACCESS_RW
                | DPL(3),
            .granularity = GRAN_4K | LONG_MODE | 0xF,
            .base_high = 0
        },
    },
    .tss = {0}
};

struct gdt *cpu_gdt;
struct gdtr *cpu_gdtr;

void *make_gdt(void)
{
    int id = get_cpu_data()->id;
    uint64_t base = (uint64_t) &cpu_tss[id];

    cpu_gdt[id] = gdt_base;

    /* iopb is disabled by having a value larger than limit
     * This disables I/O access */
    cpu_tss[id].iopb = 0xFFFF;
    cpu_tss[id].rsp0 = (uint64_t) page_alloc(PAGE_SIZE) + PAGE_SIZE;
    cpu_tss[id].ist1 = (uint64_t) page_alloc(PAGE_SIZE) + PAGE_SIZE;

    cpu_gdt[id].tss.limit_low = sizeof(struct tss);
    cpu_gdt[id].tss.base_low = base & 0xFFFF;
    cpu_gdt[id].tss.base_middle = (base >> 16) & 0xFF;
    cpu_gdt[id].tss.access = 0x89;
    cpu_gdt[id].tss.granularity = 0;
    cpu_gdt[id].tss.base_high = (base >> 24) & 0xFF;
    cpu_gdt[id].tss.base_upper = base >> 32;
    cpu_gdt[id].tss.reserved = 0;

    cpu_gdtr[id].limit = sizeof(cpu_gdt[id]) - 1;
    cpu_gdtr[id].base = (uint64_t) &cpu_gdt[id];

    return &cpu_gdtr[id];
}

void gdt_meta_init(void)
{
    cpu_gdtr = kalloc(sizeof(struct gdtr)*ncpus);
    cpu_gdt = kalloc(sizeof(struct gdt)*ncpus);
    cpu_tss = kalloc(sizeof(struct tss)*ncpus);
}

void flush_gdt(void *addr);
void gdt_init(void)
{
    flush_gdt(make_gdt());
}
