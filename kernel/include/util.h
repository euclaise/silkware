#ifndef UTIL_H
#define UTIL_H

#define _packed __attribute__((packed))

#ifdef __GNUC__
#define likely(x)   __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect((x),0)
#else
#define _packed
#define likely(x)
#define unlikely(x)
#endif

#endif
