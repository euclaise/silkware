#ifndef RAND_H
#define RAND_H

#include <types.h>
uint64_t rand64(void);
uint64_t srand_insecure(uint64_t x);
void rand_init(void);

#endif
