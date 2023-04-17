#ifndef MP_H
#define MP_H
#include <flex.h>
#include <util.h>
#include <types.h>
#include <proc.h>
#include <u.h>

int get_ncpus(void);
extern int ncpus;

struct cpu_data
{
    void *kstack;
    struct proc *proc_current;
    int32_t id;
    page_tab cur_pt;
} _packed;

extern struct cpu_data cpu_data[];

struct cpu_data *get_cpu_data(void);
void init_cpu_local(void);
void mp_init(void);
#endif
