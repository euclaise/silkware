#include <stdint.h>
#include <flex.h>

struct map_item
{
    uint64_t h;      /* Hash */
    FLEX(uint8_t) *k; /* Key */
    FLEX(uint8_t) *v; /* Value */
};

struct bucket
{
    size_t n;
    size_t cap;
    struct map_item item[];
};

typedef FLEX(struct bucket) map;
#define MAP(x, y) map
