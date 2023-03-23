ARCH ?= x86_64
TARGET := $(ARCH)-elf
CC ?= clang
LD ?= $(TARGET)-ld

CFLAGS := -target $(TARGET)   \
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
         -mcmodel=kernel

LDFLAGS := -nostdlib              \
          -static                 \
          -z max-page-size=0x1000 \
          -T arch/$(ARCH)/linker.ld

SRC_DIRS := arch/$(ARCH) libk .
SRCS := $(wildcard $(addsuffix /*.c, $(SRC_DIRS)))
OBJS := $(SRCS:.c=.o)
OPT := libk/mem.o

kern.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

OPT : CFLAGS+=-O3

%.o: %.c
	$(CC) $(CFLAGS) -I libk/include -c -o $@ $<

clean:
	rm -f $(OBJS) kern.elf

.PHONY: clean
