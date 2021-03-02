.PHONY: run

CFLAGS=-Wall -Wextra -fno-stack-protector -nostdlib -fno-pic -fno-builtin

uefi_image: src/boot/main.c
	x86_64-w64-mingw32-gcc $(CFLAGS) -Wl,--subsystem,10 -o BOOTX64.EFI $<

kernel.bin: src/kernel/main.c func.o
	clang -c -fno-pic -fno-stack-protector -nostdlib  $< -o kernel.o
	ld  -T src/kernel/kernel.ld kernel.o func.o -o $@ -M

%.o: src/kernel/%.asm
	nasm -f elf64 $< -o $@

run: uefi_image kernel.bin
	mkdir -p fs/EFI/BOOT
	cp BOOTX64.EFI fs/EFI/BOOT/
	cp kernel.bin fs/
	qemu-system-x86_64 -bios /usr/share/ovmf/x64/OVMF.fd -drive format=raw,file=fat:rw:fs -monitor stdio

clean:
	rm -rf fs BOOTX64.EFI *.o kernel.bin
