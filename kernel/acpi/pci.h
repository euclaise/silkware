#include <stdint.h>

uint8_t pci_read(
        uint8_t bus,
        uint8_t slot,
        uint8_t func,
        uint8_t offset,
        uint8_t size
    );
void pci_write(
        uint8_t bus,
        uint8_t slot,
        uint8_t func,
        uint8_t offset,
        uint32_t val,
        uint8_t size
    );
