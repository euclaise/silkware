ARCH ?= x86_64
CC := zig cc
LD := ld
ASM := clang -masm=intel -c

CFLAGS := -target $(ARCH)-freestanding  \
         -ffreestanding       \
         -fno-stack-protector \
         -fno-stack-check     \
         -fno-lto             \
         -fno-pie             \
         -fno-pic             \
         -mabi=sysv           \
         -mno-80387           \
         -mno-mmx             \
         -mno-sse             \
         -mno-sse2            \
         -mno-red-zone        \
         -mcmodel=kernel      \
		 -Ilibk/include       \
		 -I/usr/include

LDFLAGS := -nostdlib               \
          -static                  \
          -z max-page-size=0x1000  \
          -Tarch/$(ARCH)/linker.ld

SRC_DIRS := arch/$(ARCH) libk .
SRCS := $(wildcard $(addsuffix /*.c, $(SRC_DIRS))) \
		$(wildcard $(addsuffix /*.asm, $(SRC_DIRS)))
OBJS_C := $(filter %o,$(SRCS:.c=.o))
OBJS_ASM := $(filter %.o,$(SRCS:.asm=.o))
OPT := libk/mem.o

kernel.elf: $(OBJS_C) $(OBJS_ASM)
	$(LD) $(LDFLAGS) -o $@ $^

OPT : CFLAGS+=-O3

%.o: %.asm
	$(ASM) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS_C) $(OBJS_ASM) kernel.elf

.PHONY: clean
