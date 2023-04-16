#ifndef MP_H
#define MP_H
#include <flex.h>
#include <util.h>
#include <types.h>
#include <proc.h>
#include <u.h>

int get_cpuid(void);
int get_ncpus(void);
extern int ncpus;

struct cpu_data
{
    void *kstack;
    struct proc *proc_current;
    int32_t id;
} _packed;

extern int8_t cpu_data[];

struct cpu_data *get_cpu_data(void);
void init_cpu_local(void);
void mp_init(void);
#endif
