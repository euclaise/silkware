silkware.iso: build/kernel.elf
	cp limine/* build/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		build -o silkware.iso
	limine-deploy silkware.iso

build/kernel.elf: build
	cd kernel && $(MAKE)
	mv kernel/kernel.elf build/

build:
	mkdir -p build

run: silkware.iso
	qemu-system-x86_64 -serial stdio -cdrom silkware.iso 

gdb: silkware.iso
	qemu-system-x86_64 -s -S -serial stdio -no-shutdown -no-reboot \
		-cdrom silkware.iso -d int,cpu_reset

clean:
	rm -rf build
	rm -f silkware.iso
	cd kernel && $(MAKE) clean

.PHONY: run clean
