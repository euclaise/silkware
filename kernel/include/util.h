#ifndef UTIL_H
#define UTIL_H

#ifdef __GNUC__
#define likely(x)   __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect((x),0)
#define UNREACHABLE __builtin_unreachable()
#else
#define likely(x)
#define unlikely(x)
#define UNREACHABLE while (1)
#endif

#endif
