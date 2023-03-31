#ifndef UTIL_H
#define UTIL_H

#ifdef __GNUC__
#define likely(x)   __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect((x),0)
#else
#define likely(x)
#define unlikely(x)
#endif

#endif
