QEMU=qemu-system-i386

nanos.img: bootblock kernel fs.img
	dd if=/dev/zero of=nanos.img count=10000
	dd if=boot/bootblock of=nanos.img conv=notrunc
	dd if=kernel/kernel of=nanos.img seek=1 conv=notrunc

bootblock:
	@cd boot; make

kernel:
	@cd kernel; make

fs:
	touch fs

install:
	$(QEMU) -serial stdio nanos.img

debug:
	$(QEMU) -serial stdio -s -S nanos.img -gdb tcp::12345

clean:
	@cd boot; make clean
	@cd kernel; make clean
	rm -f nanos.img

.PHONY: bootblock kernel fs install debug clean
