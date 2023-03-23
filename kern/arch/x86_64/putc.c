#include <limine.h>

extern struct limine_terminal *_term;
extern limine_terminal_write _write;

void putc(char c)
{
    _write(_term, &c, 1);
}
