silkware.iso: build/kern.elf
	cp limine/* build/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		build -o silkware.iso
	limine-deploy silkware.iso

build/kern.elf: build
	cd kern && $(MAKE)
	mv kern/kern.elf build/

build:
	mkdir -p build

run: silkware.iso
	qemu-system-x86_64 -cdrom silkware.iso 

gdb: silkware.iso
	qemu-system-x86_64 -s -cdrom silkware.iso 

clean:
	rm -rf build
	rm -f silkware.iso
	cd kern && $(MAKE) clean

.PHONY: run clean
