global flush_gdt
extern gdtr
flush_gdt:
    lgdt [gdtr]
    push qword 0x8
    push .after
    retfq
.after:
    mov ax, 0x10 ; 2 << 3
    mov ds, ax
    mov es, ax
    mov ss, ax
    
    mov ax, 0x28 ; 5 << 3
    ltr ax
    ret
