global flush_gdt
extern gdtr
flush_gdt:
    cli
    lgdt [gdtr]
    push 0x8
    push .after
    retfq
.after:
    mov ax, 0x10 ; 2 << 3
    mov ds, ax
    mov es, ax
    xor rax, rax
    mov gs, ax
    
    mov ax, 0x18 ; 3 << 3
    ltr ax
    sti
    ret