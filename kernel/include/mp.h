#ifndef MP_H
#define MP_H
#include <flex.h>
#include <util.h>
#include <stdint.h>

int get_cpuid(void);
int get_ncpus(void);
extern int ncpus;

struct cpu_data
{
    void *kstack;
    int32_t id;
} _packed;

extern struct cpu_data cpu_data;

struct cpu_data *get_cpu_data(void);
void init_cpu_local(void);
#endif
