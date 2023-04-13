%include "arch/x86_64/include/arch/mp.inc"

%macro restore 1
    mov %1, [rel cpu_data + cpu_data_t.proc_state + proc_state_t.regs + regs_t.%1]
%endmacro

global return_user
extern cpu_data
return_user:
    mov ax, 0x1B ; (3 << 3) | 3, data selector 
    mov ds, ax
    mov es, ax

    ; iretq stack layout:
    ;       data selector
    ;       rsp
    ;       rflags
    ;       code selector
    ;       rip
    push 0x1B

    mov rax, [rel cpu_data + cpu_data_t.proc_state + proc_state_t.rsp]
    push rax

    pushf ; flags

    push 0x23 ; (4 << 3) | 3, code selector

    mov rax, [rel cpu_data + cpu_data_t.proc_state + proc_state_t.rip]
    push rax

    restore r15
    restore r14
    restore r13
    restore r12
    restore r11
    restore r10
    restore r9
    restore r8
    restore rbp
    restore rdi
    restore rsi
    restore rdx
    restore rcx
    restore rbx
    restore rax
    mov [rel cpu_data + cpu_data_t.kstack], rsp ; Save kernel rsp
    iretq
