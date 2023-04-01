%include "arch/x86_64/pushall.inc"

global syscall_entry
extern syscall_handler

syscall_entry:
    push rsp
    pushall
    call syscall_handler
    popall
    pop rsp
    mov r11, 0x202
    o64 sysret
