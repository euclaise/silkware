#ifndef ARCH_CTX_H
#define ARCH_CTX_H

#include <arch/proc.h>
void ctx_switch(struct irq_frame frame);

#endif
