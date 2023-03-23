#include <limine.h>
#include "serial.h"

extern int serial_ok;
void putc(char c)
{
    if (serial_ok) serial_write(c);
}
