#include <stdint.h>
int serial_init(void);
uint8_t serial_recv(void);
void serial_write(uint64_t x);
