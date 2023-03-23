#ifndef IO_H
#define IO_H

#include <stdint.h>

void putc(char c); /* Defined in arch code */
void prints(char *s);
void printx(uintptr_t x);

#endif
