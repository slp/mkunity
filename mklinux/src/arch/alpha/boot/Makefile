#
# arch/alpha/boot/Makefile
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1994 by Linus Torvalds
#

ifdef CONFIG_CROSSCOMPILE
# enable this for linking under OSF/1:
LINKFLAGS = -non_shared -T 0x20000000 -N
else
 elf=$(shell if $(LD) --help | grep elf64alpha >/dev/null; then echo yes; fi)
 ifeq ($(elf),yes)
  LINKFLAGS = -static -Ttext 0x20000000 -N
 else
  LINKFLAGS = -static -T bootloader.lds -N
 endif
endif

.S.s:
	$(CC) -D__ASSEMBLY__ -traditional -E -o $*.o $<
.S.o:
	$(CC) -D__ASSEMBLY__ -traditional -c -o $*.o $<

OBJECTS = head.o main.o
TARGETS = vmlinux.gz tools/objstrip # also needed by aboot & milo
VMLINUX = $(TOPDIR)/vmlinux
OBJSTRIP = tools/objstrip

all:	$(TARGETS)
	@echo Ready to install kernel in $(shell pwd)/vmlinux.gz

# normally no need to build these:
rawboot: vmlinux.nh tools/lxboot tools/bootlx

msb:	tools/lxboot tools/bootlx vmlinux.nh
	( cat tools/lxboot tools/bootlx vmlinux.nh ) > /dev/rz0a
	disklabel -rw rz0 'linux' tools/lxboot tools/bootlx

bootimage:	tools/mkbb tools/lxboot tools/bootlx vmlinux.nh
	( cat tools/lxboot tools/bootlx vmlinux.nh ) > bootimage
	tools/mkbb bootimage tools/lxboot

srmboot:	bootdevice bootimage
	dd if=bootimage of=$(BOOTDEV) bs=512 seek=1 skip=1
	tools/mkbb $(BOOTDEV) tools/lxboot

bootdevice:
	@test "$(BOOTDEV)" != ""  || (echo You must specify BOOTDEV ; exit -1)

vmlinux.gz: vmlinux
	gzip -fv vmlinux

#
# A raw binary without header.  Used by raw boot.
#
main.o: ksize.h

ksize.h: $(OBJSTRIP) vmlinux.nh
	echo "#define KERNEL_SIZE `$(OBJSTRIP) -p vmlinux.nh /dev/null`" > $@

vmlinux.nh: $(VMLINUX) $(OBJSTRIP)
ifeq ($(elf),yes)
	cp $(VMLINUX) vmlinux.stripped
	strip vmlinux.stripped		# work around ELF binutils bug...
	$(OBJSTRIP) -v vmlinux.stripped vmlinux.nh
	rm -f vmlinux.stripped
else
	$(OBJSTRIP) -v $(VMLINUX) vmlinux.nh
endif

vmlinux: $(TOPDIR)/vmlinux
	cp $(TOPDIR)/vmlinux vmlinux
	strip vmlinux

tools/lxboot: $(OBJSTRIP) bootloader
	$(OBJSTRIP) -p bootloader tools/lxboot

tools/bootlx: bootloader $(OBJSTRIP)
	$(OBJSTRIP) -vb bootloader tools/bootlx

$(OBJSTRIP): $(OBJSTRIP).c
	$(HOSTCC) $(OBJSTRIP).c -o $(OBJSTRIP)

tools/mkbb: tools/mkbb.c
	$(HOSTCC) tools/mkbb.c -o tools/mkbb

bootloader: $(OBJECTS)
	$(LD) $(LINKFLAGS) \
		$(OBJECTS) \
		$(LIBS) \
		-o bootloader && strip bootloader || \
		(rm -f bootloader && exit 1)

clean:
	rm -f $(TARGETS) bootloader bootimage vmlinux.nh \
		tools/mkbb tools/bootlx tools/lxboot

dep:
