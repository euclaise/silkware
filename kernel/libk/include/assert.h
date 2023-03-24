#ifndef ASSERT_H
#define ASSERT_H

void assert_test(
        int test,
        const char *file,
        int line,
        const char *func,
        const char *test_text);
#define assert(x) assert_test((x), __FILE__, __LINE__, __func__, #x)

#endif
