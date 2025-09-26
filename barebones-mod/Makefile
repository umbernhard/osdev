all: kernel boot linker iso

kernel:
	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

boot:
	i686-elf-as boot.s -o boot.o


linker:
	i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

iso:
	cp myos.bin isodir/boot/myos.bin
	grub-mkrescue -o myos.iso isodir

run:
	qemu-system-i386 -cdrom myos.iso
