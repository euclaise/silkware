%include "arch/x86_64/include/arch/pushall.inc"
%include "arch/x86_64/include/arch/mp.inc"

global syscall_entry
extern syscall_test
extern syscall_exit
extern syscall_inval
extern cpu_data

syscall_entry:
    mov r10, [rel cpu_data + cpu_data_t.kstack]
    mov [rel cpu_data + cpu_data_t.kstack], rsp ; Save process rsp
    mov rsp, r10

    push rsp
    pushall
    cmp rax, 1
    jle .cont
    call syscall_inval
.cont:
    call [jmp_tab + 8*rax]
    popall
    pop rsp

    mov r10, [rel cpu_data + cpu_data_t.kstack]
    mov [rel cpu_data + cpu_data_t.kstack], rsp ; Save kernel rsp
    mov rsp, r10

    mov r11, 0x202 ; Set RFLAGS
    o64 sysret

jmp_tab:
    dq syscall_test
    dq syscall_exit
