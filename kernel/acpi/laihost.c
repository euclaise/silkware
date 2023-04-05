#include <stdint.h>
#include <lai/host.h>
#include <kalloc.h>
#include <paging.h>
#include <io.h>
#include <panic.h>
#include <mem.h>
#include <assert.h>
#include <acpi.h>
#include <timer.h>
#include <util.h>
#include "ports.h"
#include "pci.h"

void *laihost_malloc(size_t size)
{
    return kalloc(size);
}

void laihost_free(void *ptr, size_t size)
{
    (void)size;
    kfree(ptr);
}

void *laihost_realloc(void *ptr, size_t newsize, size_t oldsize)
{
    (void)oldsize;
    return krealloc(ptr, newsize);
}

void *laihost_map(size_t address, size_t count)
{
    return kmap_phys(address, count);
}

void laihost_unmap(void *address, size_t count)
{
    kunmap(address, count);
}

void laihost_log(int level, const char *msg)
{
    (void)level;
    printf("%s\n", msg);
}

void laihost_panic(const char *msg)
{
    panic("%s", msg);
    unreachable;
}

void *laihost_scan(const char *sig, size_t index)
{
    return acpi_find(sig, index);
}

uint8_t laihost_inb(uint16_t port)
{
    return port_inb(port);
}

uint16_t laihost_inw(uint16_t port)
{
    return port_inw(port);
}

uint32_t laihost_ind(uint16_t port)
{
    return port_ind(port);
}

void laihost_outb(uint16_t port, uint8_t x)
{
    port_outb(port, x);
}

void laihost_outw(uint16_t port, uint16_t x)
{
    port_outb(port, x);
}

void laihost_outd(uint16_t port, uint32_t x)
{
    port_outd(port, x);
}

void laihost_pci_writeb(
        uint16_t seg,
        uint8_t bus,
        uint8_t slot,
        uint8_t fun,
        uint16_t offset,
        uint8_t val
    )
{
    (void)seg;
    pci_write(bus, slot, fun, offset, val, 1);
}

void laihost_pci_writew(
        uint16_t seg,
        uint8_t bus,
        uint8_t slot,
        uint8_t fun,
        uint16_t offset,
        uint16_t val
    )
{
    (void)seg;
    pci_write(bus, slot, fun, offset, val, 2);
}

void laihost_pci_writed(
        uint16_t seg, uint8_t bus,
        uint8_t slot,
        uint8_t fun,
        uint16_t offset, 
        uint32_t val
    )
{
    (void)seg;
    pci_write(bus, slot, fun, offset, val, 4);
}

uint8_t laihost_pci_readb(
        uint16_t seg,
        uint8_t bus,
        uint8_t slot,
        uint8_t fun,
        uint16_t offset
    )
{
    (void)seg;
    return pci_read(bus, slot, fun, offset, 1);
}

uint16_t laihost_pci_readw(
        uint16_t seg,
        uint8_t bus,
        uint8_t slot,
        uint8_t fun,
        uint16_t offset
    )
{
    (void)seg;
    return pci_read(bus, slot, fun, offset, 2);
}

uint32_t laihost_pci_readd(
        uint16_t seg,
        uint8_t bus,
        uint8_t slot,
        uint8_t fun,
        uint16_t offset
    )
{
    (void)seg;
    return pci_read(bus, slot, fun, offset, 4);
}

void laihost_sleep(uint64_t ms)
{
    timer_sleep_ms(ms);
}

uint64_t laihost_timer(void)
{
    return timer_count_ms();
}
