OUTPUT_FORMAT(elf64-x86-64)
ENTRY(start)

PHDRS
{
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ; /* Execute + Read */
    rodata  PT_LOAD    FLAGS((1 << 2)) ;            /* Read only */
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
}

SECTIONS
{
    . = 0xffffffff80000000;
    .text : {
        kern_load = .;
        *(.text*)
    } :text

    . = ALIGN(0x1000);

    .limine : {
        KEEP(*(.limine))
        QUAD(0)
    } :rodata

    .rodata : {
        *(.rodata*)
    } :rodata

    . = ALIGN(0x1000);

    .data : {
        *(.data*)
    } :data

    .dynamic : {
        *(.dynamic)
    } :data :dynamic

    .bss : {
        *(COMMON)
        *(.bss*)
        stack_bottom = .;
        . += 0x10000; /* Stack */
        stack_top = .;
        _kbrk_start = .;
        . += 0x1000000; /* 16MiB heap */
        kern_end  =  .;
    } :data

    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
    }
}
