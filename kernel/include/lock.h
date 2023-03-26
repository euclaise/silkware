#include <stdatomic.h>

#define DEF_LOCK(x) atomic_flag x = ATOMIC_FLAG_INIT;

#define ACQUIRE(x) \
    while (atomic_flag_test_and_set_explicit((x), memory_order_acquire)) pause()

#define RELEASE(x) atomic_flag_clear_explicit((x), memory_order_release)
