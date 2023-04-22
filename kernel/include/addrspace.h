#ifndef ADDRSPACE_H
#define ADDRSPACE_H
#include <map.h>

struct addrspace
{
    void *addr;
    map *physmap;
};

#endif
