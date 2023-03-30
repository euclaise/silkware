#include <stdint.h>
#include <flex.h>

typedef struct
{
    uint64_t h;      /* Hash */
    FLEX(uint8_t) *k; /* Key */
    FLEX(uint8_t) *v; /* Value */
} map_item;

typedef struct
{
    size_t n;
    size_t cap;
    map_item item[];
} bucket;

typedef FLEX(bucket) map;
#define MAP(x, y) map
