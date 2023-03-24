#include <stdint.h>
#include <io.h>
#include <kern.h>

typedef struct
__attribute__((packed))
{
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t  ist;
    uint8_t  attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} idt_entry_t;

__attribute__((aligned(0x10)))
static idt_entry_t idt[256];

static struct
__attribute__((packed))
{
    uint16_t limit;
    uint64_t base;
} idtr;

typedef struct
{
    uint64_t ip;
    uint64_t cs;
    uint64_t flags;
    uint64_t sp;
    uint64_t ss;
} irq_frame;


char *irq_msg[] =
{
    "EXCEPTION: Division By Zero - #0\n",
    "EXCEPTION: Debug - #1\n",
    "EXCEPTION: Non Maskable Interrupt - #2\n",
    "EXCEPTION: Breakpoint - #3\n",
    "EXCEPTION: Into Detected Overflow - #4\n",
    "EXCEPTION: Out of bounds - #5\n",
    "EXCEPTION: Invalid opcode - #6\n",
    "EXCEPTION: No Coprocessor - #7\n",
    "EXCEPTION: Double fault - #8\n",
    "EXCEPTION: Coprocessor segment overrun - #9\n",
    "EXCEPTION: Bad TSS - #10\n",
    "EXCEPTION: Segment not present - #11\n",
    "EXCEPTOIN: Stack Fault  - #12\n",
    "EXCEPTION: General protection fault - #13\n",
    "EXCEPTION: Page fault - #14\n",
    "EXCEPTION: Unknown interrupt - #15\n",
    "EXCEPTION: Coprocessor fault - #16\n",
    "EXCEPTION: Alignment check - #17\n",
    "EXCEPTION: Machine check - #18\n",
    "EXCEPTION: Reserved - #19\n",
    "EXCEPTION: Reserved - #20\n",
    "EXCEPTION: Reserved - #21\n",
    "EXCEPTION: Reserved - #22\n",
    "EXCEPTION: Reserved - #23\n",
    "EXCEPTION: Reserved - #24\n",
    "EXCEPTION: Reserved - #25\n",
    "EXCEPTION: Reserved - #26\n",
    "EXCEPTION: Reserved - #27\n",
    "EXCEPTION: Reserved - #28\n",
    "EXCEPTION: Reserved - #29\n",
    "EXCEPTION: Reserved - #30\n",
    "EXCEPTION: Reserved - #31\n"
};

__attribute__((no_caller_saved_registers))
void err_stub(int num, irq_frame *frame)
{
    printf("%s\n", irq_msg[num]);
    printf("   IP: %p\n", frame->ip);
    printf("   CS: %p\n", frame->cs);
    printf("FLAGS: %p\n", frame->flags);
    printf("   SP: %p\n", frame->sp);
    printf(   "SS: %p\n", frame->ss);

    /* if (num == 8) freeze();*/
}

#define X_ISR X(0) X(1) X(2) X(3) \
    X(4) X(5) X(6) X(7) \
    X(8) X(9) X(10) X(11) \
    X(12) X(13) X(14) X(15) \
    X(16) X(17) X(18) X(19) \
    X(20) X(21) X(22) X(23) \
    X(24) X(25) X(26) X(27) \
    X(28) X(29) X(30) X(31)

#define X(x)                           \
    __attribute__((interrupt))            \
    void irq_stub ## x (irq_frame *frame) \
    {                                     \
        err_stub(x, frame);               \
    }

X_ISR
#undef X

void idt_set(uint8_t i, void *isr, uint8_t flags)
{
	idt[i].isr_low    = (uintptr_t) isr & 0xFFFF;
	idt[i].kernel_cs  = 5 << 3;
    idt[i].ist        = 0;
    idt[i].attributes = flags;
    idt[i].isr_mid    = ((uintptr_t) isr >> 16) & 0xFFFF;
    idt[i].isr_high   = ((uintptr_t) isr >> 32) & 0xFFFFFFFF;
    idt[i].reserved   = 0;
}

void idt_init(void)
{
    idtr.base = (uint64_t) &idt[0];
    idtr.limit = sizeof(idt_entry_t) * 256 - 1;

#define X(x) idt_set(x, irq_stub ## x, 0x8E);
    X_ISR
#undef X

    __asm__ volatile ("lidt %0" : : "m"(idtr));
    __asm__ volatile ("sti");
}
