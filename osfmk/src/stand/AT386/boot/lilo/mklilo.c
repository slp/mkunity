/*
 * Copyright 1991-1998 by Open Software Foundation, Inc. 
 *              All Rights Reserved 
 *  
 * Permission to use, copy, modify, and distribute this software and 
 * its documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation. 
 *  
 * OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE. 
 *  
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR 
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT, 
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
 */
/*
 * MkLinux
 */

/*
 *  linux/tools/build.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/*
 * This file builds a disk-image from three different files:
 *
 * - bootsect: exactly 512 bytes of 8086 machine code, loads the rest
 * - setup: 8086 machine code, sets up system parm
 * - system: 80386 code for actual system
 *
 * It does some checking that all files are of the correct type, and
 * just writes the result to stdout, removing headers and padding to
 * the right amount. It also writes some system data to stderr.
 */

/*
 * Changes by tytso to allow root device specification
 */

#include <stdio.h>	/* fprintf */
#include <string.h>
#include <stdlib.h>	/* contains exit */
#include <sys/types.h>	/* unistd.h needs this */
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>	/* contains read/write */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/elf.h>
#include <linux/config.h>

#define MINIX_HEADER 32

#define N_MAGIC_OFFSET 1024

#define SYS_SIZE DEF_SYSSIZE

#define DEFAULT_MAJOR_ROOT 0
#define DEFAULT_MINOR_ROOT 0

/* max nr of sectors of setup: don't change unless you also change
 * bootsect etc */
#define SETUP_SECTS 4

#define STRINGIFY(x) #x

typedef union {
	long l;
	short s[2];
	char b[4];
} conv;

long intel_long(long l)
{
	conv t;

	t.b[0] = l & 0xff; l >>= 8;
	t.b[1] = l & 0xff; l >>= 8;
	t.b[2] = l & 0xff; l >>= 8;
	t.b[3] = l & 0xff; l >>= 8;
	return t.l;
}

short intel_short(short l)
{
	conv t;

	t.b[0] = l & 0xff; l >>= 8;
	t.b[1] = l & 0xff; l >>= 8;
	return t.s[0];
}

void die(char * str)
{
	fprintf(stderr,"%s\n",str);
	exit(1);
}

void usage(void)
{
	die("Usage: build bootsect setup system [rootdev] [> image]");
}

int main(int argc, char ** argv)
{
	int i,c,id, sz;
	unsigned long sys_size;
	char buf[1024];
	char major_root, minor_root;
	struct stat sb;
	unsigned char setup_sectors;
	char *base;
	Elf32_Ehdr *e;
	Elf32_Shdr *s;
   	Elf32_Phdr *p;
	Elf32_Addr  addr;

	if ((argc < 4) || (argc > 5))
		usage();
	if (argc > 4) {
		if (!strcmp(argv[4], "CURRENT")) {
			if (stat("/", &sb)) {
				perror("/");
				die("Couldn't stat /");
			}
			major_root = major(sb.st_dev);
			minor_root = minor(sb.st_dev);
		} else if (strcmp(argv[4], "FLOPPY")) {
			if (stat(argv[4], &sb)) {
				perror(argv[4]);
				die("Couldn't stat root device.");
			}
			major_root = major(sb.st_rdev);
			minor_root = minor(sb.st_rdev);
		} else {
			major_root = 0;
			minor_root = 0;
		}
	} else {
		major_root = DEFAULT_MAJOR_ROOT;
		minor_root = DEFAULT_MINOR_ROOT;
	}
	fprintf(stderr, "Root device is (%d, %d)\n", major_root, minor_root);
	for (i=0;i<sizeof buf; i++) buf[i]=0;
	if ((id=open(argv[1],O_RDONLY,0))<0)
		die("Unable to open 'boot'");
	if (read(id,buf,MINIX_HEADER) != MINIX_HEADER)
		die("Unable to read header of 'boot'");
	if (((long *) buf)[0]!=intel_long(0x04100301))
		die("Non-Minix header of 'boot'");
	if (((long *) buf)[1]!=intel_long(MINIX_HEADER))
		die("Non-Minix header of 'boot'");
	if (((long *) buf)[3] != 0)
		die("Illegal data segment in 'boot'");
	if (((long *) buf)[4] != 0)
		die("Illegal bss in 'boot'");
	if (((long *) buf)[5] != 0)
		die("Non-Minix header of 'boot'");
	if (((long *) buf)[7] != 0)
		die("Illegal symbol table in 'boot'");
	i=read(id,buf,sizeof buf);
	fprintf(stderr,"Boot sector %d bytes.\n",i);
	if (i != 512)
		die("Boot block must be exactly 512 bytes");
	if ((*(unsigned short *)(buf+510)) != (unsigned short)intel_short(0xAA55))
		die("Boot block hasn't got boot flag (0xAA55)");
	buf[508] = (char) minor_root;
	buf[509] = (char) major_root;	
	i=write(1,buf,512);
	if (i!=512)
		die("Write call failed");
	close (id);
	
	if ((id=open(argv[2],O_RDONLY,0))<0)
		die("Unable to open 'setup'");
	if (read(id,buf,MINIX_HEADER) != MINIX_HEADER)
		die("Unable to read header of 'setup'");
	if (((long *) buf)[0]!=intel_long(0x04100301))
		die("Non-Minix header of 'setup'");
	if (((long *) buf)[1]!=intel_long(MINIX_HEADER))
		die("Non-Minix header of 'setup'");
	if (((long *) buf)[3] != 0)
		die("Illegal data segment in 'setup'");
	if (((long *) buf)[4] != 0)
		die("Illegal bss in 'setup'");
	if (((long *) buf)[5] != 0)
		die("Non-Minix header of 'setup'");
	if (((long *) buf)[7] != 0)
		die("Illegal symbol table in 'setup'");
	for (i=0 ; (c=read(id,buf,sizeof buf))>0 ; i+=c )
		if (write(1,buf,c)!=c)
			die("Write call failed");
	if (c != 0)
		die("read-error on 'setup'");
	close (id);
	setup_sectors = (unsigned char)((i + 511) / 512);
	/* for compatibility with LILO */
	if (setup_sectors < SETUP_SECTS)
		setup_sectors = SETUP_SECTS;
	fprintf(stderr,"Setup is %d bytes.\n",i);
	for (c=0 ; c<sizeof(buf) ; c++)
		buf[c] = '\0';
	while (i < setup_sectors * 512) {
		c = setup_sectors * 512 - i;
		if (c > sizeof(buf))
			c = sizeof(buf);
		if (write(1,buf,c) != c)
			die("Write call failed");
		i += c;
	}
	
	if ((id=open(argv[3],O_RDONLY,0))<0)
		die("Unable to open 'system'");
	if (fstat(id, &sb) < 0)
	  	die("Unable to fstat 'system'");
	base = mmap(0, sb.st_size, PROT_READ,
		    MAP_PRIVATE|MAP_FILE|MAP_VARIABLE, id, 0);
	if (base == (char *) -1)
	  	die("Unable to mmap 'system'");
	e = (Elf32_Ehdr *) base;
	if (e->e_ehsize != sizeof(*e) || e->e_shnum == 0)
		die("incorrect elf format for 'system'");
	s = (Elf32_Shdr *) (base + e->e_shoff);
	for (i = 0, sz = 0; i < e->e_shnum; s++, i++) {
#if 0
		fprintf(stderr,"System section %d type %d : %x bytes @ %x off %x\n",
			i, s->sh_type, s->sh_size, s->sh_addr, s->sh_offset);
#endif
		if (s->sh_type != SHT_PROGBITS)
			continue;
		if (!sz )
			addr = s->sh_addr;
		if (s->sh_addr < addr) 
			die("System file inconsistency");
		else if (s->sh_addr > addr) {
			fprintf(stderr,"Gap %d bytes\n", s->sh_addr-addr);
			write_zeroes(s->sh_addr-addr);
		}
		fprintf(stderr,"System section %d %d bytes\n", i, s->sh_size);
		if (write(1, base + s->sh_offset, s->sh_size) != s->sh_size)
			die("write failed");
		sz += s->sh_addr+s->sh_size-addr;
		addr = s->sh_addr+s->sh_size;
		if ((sz + 15) / 16 > SYS_SIZE)
			die("System is too big");
	}
	close(id);
	if (lseek(1, 497, 0) == 497) {
		if (write(1, &setup_sectors, 1) != 1)
			die("Write of setup sectors failed");
	}
	if (lseek(1,500,0) == 500) {
		buf[0] = (sys_size & 0xff);
		buf[1] = ((sys_size >> 8) & 0xff);
		if (write(1, buf, 2) != 2)
			die("Write failed");
	}
	return(0);
}

write_zeroes(size)
{
	char buf[1024];
	register i, l;

	for (i=0;i<sizeof buf; i++) buf[i]=0;
	while(size > 0) {
	  	l = (size < 1024) ? size : 1024;
		write(1, buf, l);
		size -= l;
	}
}


