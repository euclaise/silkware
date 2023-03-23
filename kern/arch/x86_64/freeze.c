void freeze(void)
{
    while (1)
        __asm__ volatile (
                "cli\n"
                "hlt"
            );
}
