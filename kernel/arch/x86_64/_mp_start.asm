global _mp_start
extern miniheap_alloc
extern mp_start

_mp_start:
    mov rdi, 0x1000
    call miniheap_alloc
    mov rsp, rax
    call mp_start
