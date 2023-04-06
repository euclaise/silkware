QEMUFLAGS := -serial stdio -smp cores=2,threads=2

silkware.iso: root/kernel.elf
	cp kernel/external/limine/* root/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		root -o silkware.iso
	limine-deploy silkware.iso

root/kernel.elf: root
	cd kernel && $(MAKE)
	mv kernel/kernel.elf root/

root:
	mkdir -p root

run: silkware.iso
	qemu-system-x86_64 $(QEMUFLAGS) -cdrom silkware.iso 

gdb: silkware.iso
	qemu-system-x86_64 -s -S $(QEMUFLAGS) -no-shutdown -no-reboot \
		-cdrom silkware.iso -d int,cpu_reset

clean:
	rm -rf root
	rm -f silkware.iso
	cd kernel && $(MAKE) clean

.PHONY: run clean
