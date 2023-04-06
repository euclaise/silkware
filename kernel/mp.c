#include <mp.h>
int ncpus;
struct cpu_data cpu_main;

struct cpu_data *get_cpu_data(void)
{
    return &cpu_main;
}
