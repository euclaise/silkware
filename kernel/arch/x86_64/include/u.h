#ifndef U_H
#define U_H

#define PAGE_SIZE (0x1000)
#define ALLOC_ALIGN (16)

#ifdef __GNUC__
#define __ADDR__ __builtin_return_address(0)
#else
#define __ADDR__ 0
#endif

#endif
