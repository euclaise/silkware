#include <io.h>

#ifdef __GNU_C__
#define __ADDR__ __builtin_return_address(0)
#else
#define __ADDR__ 0
#endif