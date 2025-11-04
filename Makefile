############################################
#					M12 OS		
############################################

CC = x86_64-linux-gnu-gcc
AS = nasm
LD = x86_64-linux-gnu-ld

CFLAGS = -m64 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -c -Ilib -Ikernel -I.
LDFLAGS = -m elf_x86_64 -T kernel/kernel.ld --oformat binary

.PHONY: all clean run debug

all: clean disk.img run

boot.bin:
	cd boot && $(AS) -f bin -o ../boot.bin boot.asm

############################################

#SRCS = kernel/main.c kernel/vga.c lib/string.c
SRCS = $(shell find . -name "*.c" -type f)
OBJS = $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

Kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

############################################

disk.img: boot.bin Kernel.bin
	@echo "Creating disk image..."
	dd if=/dev/zero of=$@ bs=1M count=16
	dd if=boot.bin of=$@ conv=notrunc
	dd if=Kernel.bin of=$@ conv=notrunc seek=1
	@echo "Disk image created successfully!"

run: disk.img
	@echo "Starting QEMU..."
	qemu-system-x86_64 -drive file=disk.img,format=raw,index=0,media=disk -no-reboot

debug: disk.img
	@echo "Starting QEMU in debug mode..."
	qemu-system-x86_64 -drive file=disk.img,format=raw,index=0,media=disk -d cpu_reset -D qemu.log -s -S

clean:
	@echo "Cleaning up..."
	rm -f $(OBJS) *.bin disk.img qemu.log
	@echo "Clean completed!"