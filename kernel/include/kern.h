#ifndef UTIL_H
#define UTIL_H

extern char kern_load[];
void freeze(void);
void main(void);
void sbrk_init(void);
void arch_init(void);
void pause(void);

#endif
