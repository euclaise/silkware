#include <stdint.h>
#include <mem.h>
#include <kern.h>

#define ACCESS_PRESENT  1 << 6
#define ACCESS_DPL      1 << 5
#define ACCESS_NOTSYS   1 << 4
#define ACCESS_EXEC     1 << 3
#define ACCESS_DC       1 << 2
#define ACCESS_RW       1 << 1
#define ACCESS_ACCESSED 1 << 0


#define GRAN_4K   1 << 7
#define SZ_32     1 << 6
#define LONG_MODE 1 << 5

typedef struct gdt_desc
{
    uint16_t limit_low;

    uint16_t base_low;
    uint8_t base_mid;

    uint8_t access;

    uint8_t granularity;

    uint8_t base_high;
} __attribute__((packed)) gdt_desc;

struct gdtr
{
    uint16_t limit;
    uint64_t ptr;
} __attribute__((packed)) gdtr;

static struct gdt_desc gdt_descs[2];

void setup_gdt(void)
{
    memset(&gdt_descs[0], 0, sizeof(gdt_descs[0]));
    
    gdt_descs[1].limit_low = 0xFFFF;
    gdt_descs[1].base_low = (uint64_t) kern_load & 0xFFFF;
    gdt_descs[1].base_mid = ((uint64_t) kern_load >> 16) & 0xFF;
    gdt_descs[1].access = ACCESS_PRESENT | ACCESS_NOTSYS | ACCESS_EXEC | ACCESS_RW;
    gdt_descs[1].granularity = GRAN_4K | LONG_MODE | 0xF;
    gdt_descs[1].base_high = ((uint64_t) kern_load >> 24) & 0xFF;    

    gdt_descs[1].limit_low = 0xFFFF;
    gdt_descs[1].base_low = (uint64_t) kern_load & 0xFFFF;
    gdt_descs[1].base_mid = ((uint64_t) kern_load >> 16) & 0xFF;
    gdt_descs[1].access = ACCESS_PRESENT | ACCESS_NOTSYS | ACCESS_RW;
    gdt_descs[1].granularity = GRAN_4K | SZ_32 | 0xF;
    gdt_descs[1].base_high = ((uint64_t) kern_load >> 24) & 0xFF;

    gdtr.ptr = (uintptr_t) &gdt_descs;
    gdtr.limit = sizeof(gdt_descs) - 1;
    __asm__ volatile ("lgdt %0" : : "m" (gdtr));
}
