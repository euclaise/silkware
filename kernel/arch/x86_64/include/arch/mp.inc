struc regs_t
    .r15: resq 1
    .r14: resq 1
    .r13: resq 1
    .r12: resq 1
    .r11: resq 1
    .r10: resq 1
    .r9: resq 1
    .r8: resq 1
    .rbp: resq 1
    .rdi: resq 1
    .rsi: resq 1
    .rdx: resq 1
    .rcx: resq 1
    .rbx: resq 1
    .rax: resq 1
endstruc

struc proc_state_t
    .regs: resb regs_t_size
    .rsp: resq 1
    .rip: resq 1
endstruc

struc cpu_data_t
    .kstack: resq 1
    .proc_state: resq 1 ; ptr->proc_state_t_size
endstruc
