extern stack_top
extern start

section .text
global _start
_start:
    mov rsp, stack_top
    mov rax, 0
    push rax
    push rax
    call start
