global get_cpu_data
extern cpu_data
get_cpu_data:
    lea rax, [rel cpu_data]
    ret
