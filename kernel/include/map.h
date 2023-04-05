#ifndef MAP_H
#define MAP_H
#include <stdint.h>
#include <flex.h>

struct map_item
{
    uint64_t h;      /* Hash */
    FLEX(int8_t) *k; /* Key */
    FLEX(int8_t) *v; /* Value */
} _packed;

typedef struct
{
    size_t n;
    size_t cap;
    struct map_item cell[1];
} _packed map;
#define MAP(x, y) map
map *map_new(size_t n);
map *map_grow(map *m, size_t n);
void map_set(map **m, const void *k, size_t kn, const void *v, size_t vn);
void map_free(map *m);
void *map_get(const map *m, const void *k, size_t kn);
void map_del(map *m, const void *k, size_t kn);
#endif
