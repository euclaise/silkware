global check_rdseed
check_rdseed:
    push rbx
    mov rax, 7
    mov rcx, 0
    cpuid
    shr rbx, 18
    and rbx, 1
    mov rax, rbx
    pop rbx
    ret

global rdseed
rdseed:
    mov rcx, 1000
.retry:
    rdseed rax
    jc .done
    loop .retry
.done:
    ret
