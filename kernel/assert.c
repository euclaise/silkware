#include <io.h>
#include <kern.h>
#include <panic.h>

void assert_test(
        int test,
        const char *file,
        int line,
        const char *func,
        const char *test_text)
{
    if (!test)
        panic("%s:%d in %s: Assertion `%s' failed",
                file,
                line,
                func,
                test_text
            );
    return;
}

void assert_eq_test(
        long long x,
        long long y,
        const char *file,
        int line,
        const char *func,
        const char *test_text)
{
    if (x != y)
    {
        printf("\nASSERTION FAILED: %lld == %lld", x, y);
        panic("%s:%d in %s: Assertion `%s' failed",
                file,
                line,
                func,
                test_text
            );
    }
    return;
}
