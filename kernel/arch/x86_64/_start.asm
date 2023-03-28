extern stack_top
extern start

section .text
global _start
_start:
    mov rsp, stack_top
    xor rbp, rbp
    call start
