ARCH ?= x86_64
CC := zig cc
LD := ld
ASM := nasm -felf64

CFLAGS := -target $(ARCH)-freestanding  \
		 -Wall                   \
		 -Wextra                 \
         -ffreestanding          \
		 -fno-strict-aliasing    \
         -fno-stack-protector    \
		 -fno-omit-frame-pointer \
         -fno-stack-check        \
         -fno-lto                \
         -fno-pie                \
         -fno-pic                \
         -mabi=sysv              \
         -mno-80387              \
         -mno-mmx                \
         -mno-sse                \
         -mno-sse2               \
         -mno-red-zone           \
         -mcmodel=kernel         \
		 -Iinclude               \
		 -Iarch/$(ARCH)/include  \
		 -Iacpi/include          \
		 -Iexternal/lai/lai/include

LDFLAGS := -nostdlib              \
          -static                 \
          -z max-page-size=0x1000 \
          -Tarch/$(ARCH)/linker.ld

SRC_DIRS := arch/$(ARCH) acpi .
SRCS := $(wildcard $(addsuffix /*.c, $(SRC_DIRS))) \
		$(wildcard $(addsuffix /*.asm, $(SRC_DIRS))) \
		external/fonts/font.c
OBJS_C := $(filter %o,$(SRCS:.c=.o))
OBJS_ASM := $(filter %.o,$(SRCS:.asm=.o))
OPT := mem.o

OPT : CFLAGS+=-O3

kernel.elf: $(OBJS_C) $(OBJS_ASM) external/lai/lai.a
	$(LD) $(LDFLAGS) -o $@ $^

external/lai/lai.a:
	cd external/lai && $(MAKE)


%.o: %.asm
	$(ASM) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(OBJS_C) $(OBJS_ASM) kernel.elf
	cd external/lai && $(MAKE) clean

