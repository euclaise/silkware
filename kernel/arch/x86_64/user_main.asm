global user_main

align 0x1000
user_main:
    push 1
    mov rax, 0
    mov rdi, 1
    syscall
a:  jmp a
