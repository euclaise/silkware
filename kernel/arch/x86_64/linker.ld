OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

PHDRS
{
    text   PT_LOAD FLAGS((1 << 0) | (1 << 2)) ; /* Execute + Read */
    rodata PT_LOAD FLAGS((1 << 2)) ;            /* Read only */
    data   PT_LOAD FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
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
        . = ALIGN(16);
        stack_bottom = .;
        . += 0x10000; /* Stack */
        stack_top = .;

        . = ALIGN(0x1000);
        page_tmp = .;
        . += 0x1000;

        miniheap_start = .;
        . += 0x64000; /* Bootstrap mini-heap */
        miniheap_end = .;
        kern_end  =  .;
    } :data

    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
    }
}
