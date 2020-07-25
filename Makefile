CC = ~/opt/cross/bin/x86_64-elf-gcc
CFLAGS = -Wall -Wextra -ffreestanding -fno-pic -mno-sse -mno-sse2 -mno-mmx -mno-80387 -fno-stack-protector -I. -mno-red-zone -gdwarf -mcmodel=kernel
QEMUFLAGS = -m 1G -vga vmware -serial file:serial.log -soundhw pcspk -smp 4 -no-reboot

LDFLAGS = -O2 -nostdlib -no-pie -lgcc -static-libgcc
CSOURCE = $(shell find ./ -type f -name '*.c' | sort)

build:
	rm -f seaweed
	mkdir -p Bin
	$(CC) $(CFLAGS) -c $(CSOURCE)
	nasm -felf64 kernel/int/isr.asm -o isr.o
	$(CC) $(LDFLAGS) -T linker.ld *.o -o Bin/kernel.bin
	nasm -fbin kernel/boot.asm -o seaweed
	rm -f *.o Bin/kernel.bin 
clean:
	rm -f $(OBJSOURCE) kernel.bin seaweed serial.log qemu.log

qemu: build
	touch serial.log
	qemu-system-x86_64 $(QEMUFLAGS) seaweed &
	tail -n0 -f serial.log

info: build
	qemu-system-x86_64 $(QEMUFLAGS) seaweed -no-reboot -monitor stdio -d int -D qemu.log -no-shutdown

debug: build
	qemu-system-x86_64 $(QEMUFLAGS) seaweed -no-reboot -monitor stdio -d int -no-shutdown
