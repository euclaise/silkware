%include "arch/x86_64/include/arch/mp.inc"
global call_user
extern cpu_data
call_user:
    mov rcx, 0x100000
    mov r11, 0x202
    mov rdi, 100
    lea rax, [rel cpu_data + CPU_STACK_OFF]
    mov [rax], rsp
    mov rsp, 0xFFFFF000
    o64 sysret
