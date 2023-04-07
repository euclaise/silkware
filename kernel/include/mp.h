#ifndef MP_H
#define MP_H
#include <flex.h>
#include <util.h>

int get_cpuid(void);
int get_ncpus(void);
extern int ncpus;

struct cpu_data
{
    struct cpu_data *self;
    int id;
} _packed;

extern struct cpu_data cpu_main;
struct cpu_data *get_cpu_data(void);
void init_cpu_local(void);
#endif
