#include <stdint.h>
#include <stddef.h>
#include <mem.h>

void *memmove(void *dst, const void *src, size_t n)
{
    uint8_t *dst8 = dst;
    const uint8_t *src8 = src;

    if (src == dst || n == 0) return dst;

    if (src < dst) while (n--) dst8[n] = src8[n];
    else
    {
        size_t i = 0;
        for (i = 0; i < n; ++i) dst8[i] = src8[i];
    }
    return dst;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    return memmove(dst, src, n);
}

void *memset(void *dst, int c, size_t n)
{
    uint8_t *dst8 = dst;
    while (--n) dst8[n] = c;
    return dst;
}

int memcmp(const void *p1, const void *p2, size_t n)
{
    const uint8_t *s1 = p1, *s2 = p2;
    while (n--) if (s1[n] != s2[n]) return s1[n] < s2[n] ? -1 : 1;
    return 0;
}
