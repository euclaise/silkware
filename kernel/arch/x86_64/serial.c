#include <arch/ports.h>

#define COM1 0x3F8

int serial_ok;

void serial_init(void)
{
    /* https://wiki.osdev.org/Serial_Ports#Initialization */
    port_outb(COM1 + 1, 0x00); /* Disable IRQs */
    port_outb(COM1 + 3, 0x80); /* Enable DLAB for setting baud rate */
    port_outb(COM1 + 0, 0x03); /* 38400 baud divisor (3) high byte */
    port_outb(COM1 + 1, 0x00); /*                        low byte */
    port_outb(COM1 + 3, 0x03); /* Disable DLAB, etc */
    port_outb(COM1 + 2, 0xC7); /* Enable FIFO, 14 byte threshold */
    port_outb(COM1 + 4, 0x0B); /* IRQs enabled, RTS/DSR set */
    port_outb(COM1 + 4, 0x1E); /* Loopback mode, test chip */

    port_outb(COM1, 0x16); /* Test message */
    if (port_inb(COM1) != 0x16) return;

    port_outb(COM1 + 4, 0x0F); /* Set normal operation mode */
    serial_ok = 1;
}

uint8_t serial_recv(void)
{
    while (!(port_inb(COM1 + 5) & 1)); /* Wait for byte to be ready */
    return port_inb(COM1);
}

void serial_write(uint64_t x)
{
    while (!(port_inb(COM1 + 5) & 0x20)); /* Wait for clear transport */
    port_outb(COM1, x);
}
