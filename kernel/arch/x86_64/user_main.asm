global user_main

align 0x1000
user_main:
    mov rax, 0
    syscall
a:  jmp a
