#ifndef KERN_H
#define KERN_H

#ifdef __GNUC__
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

extern char kern_load[];
NORETURN void freeze(void);
void main(void);
void arch_init(void);
void pause(void);
void backtrace(void);
void init_syscalls(void);

#endif
