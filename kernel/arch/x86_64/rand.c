/* NON-SECURE RNG */
#include <rand.h>
#include <stdbool.h>
#include <stdint.h>
#include <arch/hpet.h>

bool has_rdseed;

int check_rdseed(void);
uint64_t rdseed(void);
uint64_t read_hpet(void);
uint64_t rdtsc(void);

void rand_init(void)
{
    if (check_rdseed()) has_rdseed = true;
}

uint64_t rand64(void)
{
    if (has_rdseed) return rdseed();
    return read_hpet();
}
