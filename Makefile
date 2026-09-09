
all: os-image
	bochs
	echo "made"

OBJS = kernel/kernel_entry.o kernel/kernel.o drivers/screen.o kernel/low_level.o cpu/idt.o cpu/interrupt.o cpu/isr.o cpu/irq.o drivers/keyboard.o

os-image : boot_sect.bin kernel.bin
	cat $^ > $@

boot_sect.bin : boot/boot_sect.asm
	nasm $^ -o $@

kernel.bin : $(OBJS)
	ld -m elf_i386 -Ttext 0x1000 --oformat binary -s $^ -o $@

%.o: %.c
	gcc -m32 -fno-pie -ffreestanding -c $< -o $@
%.o: %.asm
	nasm $< -f elf -o $@
