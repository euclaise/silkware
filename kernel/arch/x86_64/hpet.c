#include <panic.h>
#include <acpi.h>
#include <acpispec/tables.h>
#include <paging.h>
#include <timer.h>

static struct hpet_tab
{
    acpi_header_t h;
    uint8_t revision;
    uint8_t comparator_count : 5;
    uint8_t counter_size : 1;
    uint8_t reserved : 1;
    uint8_t legacy_replacement : 1;
    uint16_t pci_vendor_id;
    acpi_gas_t addr;
    uint8_t hpet_num;
    uint16_t min_tick;
    uint8_t page_prot;
} __attribute__((packed)) *hpet_tab;

#define HPET_MAX_TIMERS (32)

static volatile struct hpet
{
    uint64_t cap; /* Capabilities */
    uint64_t reserved0;
    uint64_t config;
    uint64_t reserved1;
    uint64_t isr;
    uint64_t reserved2[25];
    uint64_t main_ctr;
    struct hpet_timer
    {
        uint64_t config;
        uint64_t ctr;
        uint64_t fsb[2];
    } hpet_timers[HPET_MAX_TIMERS];
} *hpet;

static uint32_t clk = 0;
static bool hpet_ready;

void hpet_init(void)
{
    hpet_tab = acpi_find("HPET", 0);
    if (!hpet_tab) panic("silkware requires an HPET, could not find one\n");

    hpet = kmap_phys((void *) hpet_tab->addr.base, sizeof(struct hpet));

    printf("HPET at %p (phys=%p)\n", hpet, hpet_tab->addr.base);

    clk = hpet->cap >> 32;

    hpet->config = 0;
    hpet->main_ctr = 0;
    hpet->config = 1;
    hpet_ready = true;
}

uint64_t read_hpet(void)
{
    if (!hpet_ready) panic("Tried to read HPET before initialization");
    return hpet->main_ctr;
}

void timer_sleep_us(uint64_t us)
{
    uint64_t end = read_hpet() + (us * 1000000000) / clk;
    uint64_t cur;
    while ((cur = read_hpet()) < end);
}

void timer_sleep_ms(uint64_t ms)
{
    timer_sleep_us(ms * 1000);
}

uint64_t timer_count_ms(void)
{
    return read_hpet() / 1000;
}
