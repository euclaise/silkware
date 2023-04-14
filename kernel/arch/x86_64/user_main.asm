global user_main1

align 0x1000
user_main1:
.a:
    mov rdi, 1
    mov rax, 0
    syscall

    mov rax, 1
    syscall
    jmp .a
