#include <stdatomic.h>

void pause(void);
typedef atomic_flag lock_t;
#define LOCK_INIT ATOMIC_FLAG_INIT

#define ACQUIRE(x) \
    while (atomic_flag_test_and_set_explicit((x), memory_order_acquire)) pause()

#define RELEASE(x) atomic_flag_clear_explicit((x), memory_order_release)
