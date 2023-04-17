global get_cpu_data
extern cpu_data
get_cpu_data:
    mov rax, [gs:0]
    ret
