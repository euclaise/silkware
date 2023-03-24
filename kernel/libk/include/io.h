#ifndef IO_H
#define IO_H

#include <stdint.h>

void serial_init(void);

void putc(char c); /* Defined in arch code */
void printf(char *f, ...);


#endif
