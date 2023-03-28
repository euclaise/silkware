#ifndef IO_H
#define IO_H

#include <stdint.h>

#define COLOR_RED   ((char)0xFE)
#define COLOR_WHITE ((char)0xFF)

void serial_init(void);

void putc(char c); /* Defined in arch code */
void printf(char *f, ...);

#endif
