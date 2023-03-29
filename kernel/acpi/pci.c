#include <stdint.h>
#include <io.h>
#include "ports.h"

static inline uint32_t pciaddr(
        uint8_t bus,
        uint8_t slot,
        uint8_t func,
        uint8_t offset
    )
{
    return (uint32_t)(
        (bus << 16)
        | (slot << 11)
        | (func << 8)
        | (offset & 0xFC)
        | ((uint32_t) 0x80000000)
    );
}

uint8_t pci_read(
        uint8_t bus,
        uint8_t slot,
        uint8_t func,
        uint8_t offset,
        uint8_t size
    )
{
    port_outd(0xCF8, pciaddr(bus, slot, func, offset));
    switch (size)
    {
    case 1: return port_inb(0xCFC + (offset & 3));
    case 2: return port_inw(0xCFC + (offset & 2));
    case 4: return port_ind(0xCFC);
    default:
        printf("Invalid PCI access size (read): %d\n", size);
        return 0;
    }
}

void pci_write(
        uint8_t bus,
        uint8_t slot,
        uint8_t func,
        uint8_t offset,
        uint32_t val,
        uint8_t size
    )
{
    port_outd(0xCF8, pciaddr(bus, slot, func, offset));
    switch (size)
    {
    case 1:
        port_outb(0xCFC + (offset & 3), val);
        break;
    case 2:
        port_outw(0xCFC + (offset & 2), val);
        break;
    case 4:
        port_outd(0xCFC, val);
        break;
    default:
        printf("Invalid PCI access size (write): %d\n", size);
    }
}
