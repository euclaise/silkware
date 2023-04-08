%include "arch/x86_64/include/arch/pushall.inc"

global syscall_entry
extern syscall_test
extern syscall_inval

syscall_entry:
    push rsp
    pushall
    cmp rax, 0
    jle .cont
    call syscall_inval
.cont:
    call [jmp_tab + 8*rax]
    popall
    pop rsp
    mov r11, 0x202
    o64 sysret

jmp_tab:
    dq syscall_test
