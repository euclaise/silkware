%include "arch/x86_64/include/arch/pushall.inc"

extern lapic_eoi

extern isr_handle
%macro isr_stub 0
    pushall
    cld
    mov rdi, rsp
    call isr_handle
    call lapic_eoi
    popall
    add rsp, 16
    iretq
%endmacro

%macro isr 1
    align 4
    global isr%1
    isr%1:
        push 0
        push %1
        isr_stub
%endmacro

%macro isr_err 1
    align 4
    global isr%1
    isr%1:
        push %1
        isr_stub
%endmacro

%define iserr(i) (i == 8 || (i >= 10 && i <= 14) || i == 17 || i == 21 || i == 29 || i == 30)


%assign i 0
%rep 256
%if iserr(i)
    isr_err i
%else
    isr i
%endif
%assign i i + 1
%endrep
