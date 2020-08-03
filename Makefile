CC = ~/opt/cross/bin/x86_64-elf-gcc
CFLAGS = -Wall -Wextra -ffreestanding -fno-pic -mno-sse -mno-sse2 -mno-mmx -mno-80387 -fno-stack-protector -I. -Ilib -mno-red-zone -gdwarf -mcmodel=kernel
QEMUFLAGS = -m 4G -vga vmware -serial file:serial.log -soundhw pcspk -smp 4 -enable-kvm 

LDFLAGS = -O2 -nostdlib -no-pie -lgcc -static-libgcc
CSOURCE = $(shell find ./ -type f -name '*.c' | sort)

build:
	rm -f seaweed
	mkdir -p Bin
	$(CC) $(CFLAGS) -c $(CSOURCE)
	nasm -felf64 kernel/int/isr.asm -o isr.o
	nasm -felf64 kernel/sched/scheduler.asm -o schedulerASM.o
	nasm -felf64 kernel/int/gdt.asm -o gdtASM.o
	nasm -felf64 lib/libu/syscalls.asm -o syscalls.o
	$(CC) $(LDFLAGS) -T linker.ld *.o -o Bin/kernel.bin
	nasm -fbin kernel/boot.asm -o seaweed.img
	rm -f *.o Bin/kernel.bin
clean:
	rm -f $(OBJSOURCE) kernel.bin seaweed.img serial.log qemu.log

qemu: build
	touch serial.log
	qemu-system-x86_64 $(QEMUFLAGS)  -drive id=disk,file=seaweed.img,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0 -M q35 &
	tail -n0 -f serial.log

info: build
	qemu-system-x86_64 seaweed.img $(QEMUFLAGS) -monitor stdio -d int -D qemu.log -no-shutdown -no-reboot

debug: build
	qemu-system-x86_64 -no-reboot -monitor stdio -d int -no-shutdown $(QEMUFLAGS) seaweed.img
