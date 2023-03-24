#include <stdint.h>

typedef struct
__attribute__((packed))
{
     uint64_t present      : 1;  /* 0: Page is present? */
     uint64_t writable     : 1;  /* 1: Writable? */
     uint64_t user         : 1;  /* 2: User-accessable? */
     uint64_t writethrough : 1;  /* 3: Write-through */
     uint64_t nocache      : 1;  /* 4: Cache disable? */
     uint64_t accessed     : 1;  /* 5: Accessed? */
     uint64_t dirty        : 1;  /* 6: Dirty? */
     uint64_t pat          : 1;  /* 7: Has page attribute table? */
     uint64_t global       : 1;  /* 8: Global */
     uint64_t avl1         : 3;  /* 9-11: Unused */
     uint64_t addr         : 36; /* 12-47 */
     uint64_t zero         : 4;  /* 48-51 */
     uint64_t avl2         : 7;  /* 52-58 */
     uint64_t protkey      : 4;  /* 58-62 */
     uint64_t noexec       : 1;  /* 63 */
} pte_t;
