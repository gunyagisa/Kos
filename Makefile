.PHONY: run


uefi_image: main.c
	x86_64-w64-mingw32-gcc -Wall -Wextra -nostdlib -fno-builtin -Wl,--subsystem,10 -o BOOTX64.EFI main.c

run: uefi_image
	mkdir -p fs/EFI/BOOT
	cp BOOTX64.EFI fs/EFI/BOOT/
	qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -drive format=raw,file=fat:rw:fs
