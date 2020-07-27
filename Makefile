CC = ~/opt/cross/bin/x86_64-elf-gcc
CFLAGS = -Wall -Wextra -ffreestanding -fno-pic -mno-sse -mno-sse2 -mno-mmx -mno-80387 -fno-stack-protector -I. -mno-red-zone -gdwarf -mcmodel=kernel
QEMUFLAGS = -m 4G -vga vmware -serial file:serial.log -soundhw pcspk -smp cpus=4 

LDFLAGS = -O2 -nostdlib -no-pie -lgcc -static-libgcc
CSOURCE = $(shell find ./ -type f -name '*.c' | sort)

build:
	rm -f seaweed
	mkdir -p Bin
	$(CC) $(CFLAGS) -c $(CSOURCE)
	nasm -felf64 kernel/int/isr.asm -o isr.o
	nasm -felf64 kernel/sched/scheduler.asm -o schedulerASM.o
	$(CC) $(LDFLAGS) -T linker.ld *.o -o Bin/kernel.bin
	nasm -fbin kernel/boot.asm -o seaweed
	dd if=/dev/zero bs=1M count=0 seek=64 of=seaweed.img
	dd if=seaweed of=seaweed.img
	rm -f *.o Bin/kernel.bin seaweed
clean:
	rm -f $(OBJSOURCE) kernel.bin seaweed.img serial.log qemu.log

qemu: build
	touch serial.log
	qemu-system-x86_64 $(QEMUFLAGS) seaweed.img &
	tail -n0 -f serial.log

info: build
	qemu-system-x86_64 seaweed.img -m 1G -no-reboot -monitor stdio -d int -D qemu.log -no-shutdown -vga vmware -enable-kvm

debug: build
	qemu-system-x86_64 -no-reboot -monitor stdio -d int -no-shutdown -m 4G -vga vmware -serial file:serial.log -soundhw pcspk -enable-kvm seaweed.img
