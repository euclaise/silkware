global user_main2

align 0x1000
user_main2:
.a:
    mov rdi, 2
    mov rax, 0
    syscall
    jmp .a
