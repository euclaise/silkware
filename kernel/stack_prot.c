#include <stdint.h>
#include <panic.h>
 
#ifdef __GNUC__
#if UINT32_MAX == UINTPTR_MAX
uintptr_t __stack_chk_guard = 0xe2dee396;
#else
uintptr_t __stack_chk_guard = 0x595e9fbd94fda766;
#endif
 
__attribute__((noreturn))
void __stack_chk_fail(void)
{
	panic("Stack overflow detected!");
}
#endif
