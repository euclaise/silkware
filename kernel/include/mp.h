#ifndef MP_H
#define MP_H
#include <flex.h>
int get_cpuid(void);
int get_ncpus(void);
extern int ncpus;

struct cpu_data
{
    struct cpu_data *self;
    int id;
};
extern struct cpu_data cpu_main;
struct cpu_data *get_cpu_data(void);
#endif
