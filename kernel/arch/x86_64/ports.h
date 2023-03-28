#include <stdint.h>

uint8_t port_inb(uint16_t port);
void port_outb(uint16_t port, uint8_t x);
uint16_t port_inw(uint16_t port);
void port_outw(uint16_t port, uint16_t x);
uint32_t port_ind(uint16_t port);
void port_outd(uint16_t port, uint32_t x);
