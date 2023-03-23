#include <stdint.h>
#include <io.h>

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
    prints(irq_msg[num]);
    prints("\n");
    prints("IP: ");
    printx(frame->ip);
    prints("\n");
    prints("CS: ");
    printx(frame->cs);
    prints("\n");
    prints("FLAGS: ");
    printx(frame->flags);
    prints("\n");
    prints("SP: ");
    printx(frame->sp);
    prints("\n");
    prints("SS: ");
    printx(frame->ss);
    prints("\n");
}

#define STUB(x)                           \
    __attribute__((interrupt))            \
    void irq_stub ## x (irq_frame *frame) \
    {                                     \
        err_stub(x, frame);               \
    }

STUB(0)
STUB(1)
STUB(2)
STUB(3)
STUB(4)
STUB(5)
STUB(6)
STUB(7)
STUB(8)
STUB(9)
STUB(10)
STUB(11)
STUB(12)
STUB(13)
STUB(14)
STUB(15)
STUB(16)
STUB(17)
STUB(18)
STUB(19)
STUB(20)
STUB(21)
STUB(22)
STUB(23)
STUB(24)
STUB(25)
STUB(26)
STUB(27)
STUB(28)
STUB(29)
STUB(30)
STUB(31)
#undef STUB

extern uintptr_t _kern_offset;

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

#define REGISTER(x) idt_set(x, irq_stub ## x, 0x8E)
    REGISTER(0);
    REGISTER(1);
    REGISTER(2);
    REGISTER(3);
    REGISTER(4);
    REGISTER(5);
    REGISTER(6);
    REGISTER(7);
    REGISTER(8);
    REGISTER(9);
    REGISTER(10);
    REGISTER(11);
    REGISTER(12);
    REGISTER(13);
    REGISTER(14);
    REGISTER(15);
    REGISTER(16);
    REGISTER(17);
    REGISTER(18);
    REGISTER(19);
    REGISTER(20);
    REGISTER(21);
    REGISTER(22);
    REGISTER(23);
    REGISTER(24);
    REGISTER(25);
    REGISTER(26);
    REGISTER(27);
    REGISTER(28);
    REGISTER(29);
    REGISTER(30);
    REGISTER(31);
#undef REGISTER

    __asm__ volatile ("lidt %0" : : "m"(idtr));
    __asm__ volatile ("sti");
}
