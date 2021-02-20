.PHONY: run


uefi_image: src/boot/main.c
	x86_64-w64-mingw32-gcc $< -Wall -Wextra -nostdlib -fno-builtin -Wl,--subsystem,10 -o BOOTX64.EFI

kernel.bin: src/kernel/main.c
	gcc -c -fno-pic -fno-stack-protector -fno-common -nostdlib $< -o kernel.o
	ld  -T src/kernel/kernel.ld kernel.o -o $@ -M

run: uefi_image kernel.bin
	mkdir -p fs/EFI/BOOT
	cp BOOTX64.EFI fs/EFI/BOOT/
	cp kernel.bin fs/
	qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -drive format=raw,file=fat:rw:fs -monitor stdio

clean:
	rm -rf fs BOOTX64.EFI kernel.o kernel.bin
