SRCS := $(wildcard lai/core/*.c) \
		  $(wildcard lai/helpers/*.c) \
		  $(wildcard lai/drivers/*.c)
OBJS := $(SRCS:.c=.o)

CC := zig cc

ARCH ?= x86_64
CFLAGS := -target $(ARCH)-freestanding  \
		 -Wall                \
		 -Wextra              \
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
		 -Ilai/include

lai.a: $(OBJS)
	$(ARCH)-elf-ar cr lai.a $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(OBJS)
	rm -f lai.a
