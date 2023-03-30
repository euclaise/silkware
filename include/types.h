#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint64_t pid_t;
typedef int32_t fd_t;

typedef struct note_t
{
    pid_t from;
    pid_t to;
    size_t len;
    int8_t msg[];
} note_t;

#endif
