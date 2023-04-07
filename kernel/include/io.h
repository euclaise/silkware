#ifndef IO_H
#define IO_H

#define COLOR_MIN   ((char)0xFD)
#define COLOR_GREEN ((char)0xFD)
#define COLOR_RED   ((char)0xFE)
#define COLOR_WHITE ((char)0xFF)

void serial_init(void);

void putc(char c); /* Defined in arch code */
void printf(char *f, ...);

#endif
