CC = gcc
AS = gas
LD = ld
CFLAGS = -m32 -fno-pic -static -fno-strict-aliasing -fno-builtin -fno-stack-protector -Wall -Werror -ggdb -O2 -I./include
ASFLAGS = $(CFLAGS)

LDFLAGS = -m elf_i386
OBJCOPY = objcopy
OBJDUMP = objdump
OBJS = main.o \
	   console.o \
	   vfprintf.o \
	   string.o \
	   memory/kvm.o \
	   memory/util.o \
	   irq/i8259.o \
	   irq/idt.o \
	   irq/irq_handle.o \
	   irq/do_irq.o \
	   process/util.o \
	   process/schedule.o

kernel: $(OBJS) Makefile
	$(LD) $(LDFLAGS) $(INCLUDE) -N -e os_init -Ttext 0xC0100000 -o kernel $(OBJS)

-include *.d

clean:
	rm -f *.d *.o kernel
	rm -f memory/*.d memory/*.o
	rm -f irq/*.d irq/*.o
	rm -f process/*.d process/*.o
