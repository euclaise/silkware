%macro pushall 0
push rax
;push rbx
push rcx
push rdx
push rdi
push rsi
push rbp
push r8
push r9
push r10
push r11
%endmacro

%macro popall 0
pop r11
pop r10
pop r9
pop r8
pop rbp
pop rsi
pop rdi
pop rdx
pop rcx
;pop rbx
pop rax
%endmacro

extern isr_handle
global isr_stub
isr_stub:
    pushall
    cld
    mov rdi, rsp
    call isr_handle
    popall
    add rsp, 24
    iretq

%macro isr 1
    align 4
    global isr%1
    isr%1:
        push 0
        push %1
        jmp isr_stub
%endmacro

%macro isr_err 1
    align 4
    global isr%1
    isr%1:
        push %1
        jmp isr_stub
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
