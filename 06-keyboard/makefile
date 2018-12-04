GCCPARAMS = -m32 -Iinclude -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings -std=c99
ASPARAMS = --32
LDPARAMS = -melf_i386


objects = obj/kernel.o \
		  obj/boot/print.o \
		  obj/boot/loader.o \
  		  obj/boot/gdt.o \
  		  obj/hardware/port.o \
  		  obj/hardware/idt.o \
  		  obj/hardware/idtstubs.o \
  		  obj/hardware/keyboard.o

run: mykernel.iso
	(killall VirtualBox && sleep 1) || true
	VirtualBox --startvm 'myos' &

obj/%.o: %.c
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -c -o $@ $<

obj/%.o: %.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

mykernel.iso: mykernel.bin
	mkdir -p iso/boot/grub
	cp mykernel.bin iso/boot/mykernel.bin
	echo 'set timeout=0'                      > iso/boot/grub/grub.cfg
	echo 'set default=0'                     >> iso/boot/grub/grub.cfg
	echo ''                                  >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/mykernel.bin'    >> iso/boot/grub/grub.cfg
	echo '  boot'                            >> iso/boot/grub/grub.cfg
	echo '}'                                 >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=mykernel.iso iso
	cp mykernel.iso ../
	rm -rf iso

install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

.PHONY: clean
clean:
	rm -rf obj mykernel.bin mykernel.iso