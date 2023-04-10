#ifndef ASSERT_H
#define ASSERT_H

void assert_test(
        int test,
        const char *file,
        int line,
        const char *func,
        const char *test_text);

void assert_eq_test(
        long long x,
        long long y,
        const char *file,
        int line,
        const char *func,
        const char *test_text);

#define assert(x) assert_test((x), __FILE__, __LINE__, __func__, #x)
#define assert_eq(x, y) assert_eq_test(    \
        (long long)(x), (long long)y, \
        __FILE__, __LINE__, __func__, \
        #x " == "#y)

#ifdef __GNUC__
#define _UNREACHABLE __builtin_unreachable()
#else
#define _UNREACHABLE while (1)
#endif

#define unreachable \
    do \
    { \
        assert_test(0, __FILE__, __LINE__, __func__, "UNREACHABLE"); \
        _UNREACHABLE; \
    } while (0)

#endif
