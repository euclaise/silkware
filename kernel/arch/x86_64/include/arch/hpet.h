#include <stdint.h>
void hpet_init(void);
void hpet_init(void);

void hpet_sleep(uint64_t us);
void hpet_sleep_ms(uint64_t ms);
uint64_t timer_count_ms(void);
