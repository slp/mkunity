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
 * Instruction disassembler.
 */

#include <mach/boolean.h>
#include <machine/db_machdep.h>

#include <ddb/db_access.h>
#include <ddb/db_sym.h>
#include <ddb/db_output.h>

#include <kern/task.h>
#include <kern/misc_protos.h>

#include "ppc_disasm.h"

db_addr_t	db_disasm_pc, db_disasm_symaddr;
boolean_t	db_disasm_print_symaddr;

/*
 * Disassemble instruction at 'loc'.  'altfmt' specifies an
 * (optional) alternate format.  Return address of start of
 * next instruction.
 */
db_addr_t
db_disasm(
	db_addr_t	loc,
	boolean_t	altfmt,
	task_t		task)
{
	int inst;
	char *p;

	inst = db_get_task_value(loc, 4, FALSE, task);
	db_disasm_pc = loc;
	db_disasm_print_symaddr = FALSE;
	p = in(inst);
	db_printf("%s", p);
	if (db_disasm_print_symaddr) {
		db_printf(" <");
		db_task_printsym(db_disasm_symaddr, DB_STGY_ANY, task);
		db_printf(">");
	}
	dis_done();
	return (loc+4);
}

/*
 * Given four bytes of instruction (stored as an int, not an
 * array of characters), compute if the instruction reads
 * memory.
 */
int
db_inst_load(
	unsigned long insw)
{
#if 1
	db_printf("db_inst_load: coming soon in a debugger near you!\n");
	return 0;
#else
	unsigned char insb, bits;

	insb = insw & 0xff;
	insw >>= 8;
	bits = db_ldstrtab[insb];
	if (!(bits & DBLS_LOAD))
		return (0);
	while (1) {
		switch (bits & DBLS_MODS) {
		case 0:
			return (1);	
		case DBLS_MODRM:
			insb = insw & 0xff;
			return ((insb & 0xc0) != 0xc0);
		case DBLS_SECOND|DBLS_MODRM:
			insb = insw & 0xff;
			return ((insb & 0xc0) != 0xc0 ? 2 : 0);
		case DBLS_SECOND:
			return (2);
		case DBLS_ESCAPE:
			insb = insw & 0xff;
			insw >>= 8;
			bits = db_ldstrtab0f[insb];
			break;
		case DBLS_SWREG:
			return (db_inst_swreg(TRUE, insw, insb));
		default:
			panic ("db_inst_load: unknown mod bits");
		}
	}
#endif
}

/*
 * Given four bytes of instruction (stored as an int, not an
 * array of characters), compute if the instruction writes
 * memory.
 */
int
db_inst_store(
	unsigned long insw)
{
#if 1
	db_printf("db_inst_store: coming soon in a debugger near you!\n");
	return 0;
#else
	unsigned char insb, bits;

	insb = insw & 0xff;
	insw >>= 8;
	bits = db_ldstrtab[insb];
	if (!(bits & DBLS_STORE))
		return (0);
	while (1) {
		switch (bits & DBLS_MODS) {
		case 0:
			return (1);	
		case DBLS_MODRM:
			insb = insw & 0xff;
			return ((insb & 0xc0) != 0xc0);
		case DBLS_SECOND|DBLS_MODRM:
			insb = insw & 0xff;
			return ((insb & 0xc0) != 0xc0 ? 2 : 0);
		case DBLS_SECOND:
			return (2);
		case DBLS_ESCAPE:
			insb = insw & 0xff;
			insw >>= 8;
			bits = db_ldstrtab0f[insb];
			break;
		case DBLS_SWREG:
			return (db_inst_swreg(FALSE, insw, insb));
		default:
			panic ("db_inst_store: unknown mod bits");
		}
	}
#endif
}

/*
 * Extra routines for the automatically generated disassembler
 */
char *
hex(
	bits n)
{
	char *p;

	if (n < 10)
		return dec(n);
	p = dis_alloc(11);
	sprintf(p, "0x%lx", n);
	return p;
}

char *
dec(
	bits n)
{
	char *p = dis_alloc(11);
	sprintf(p, "%lu", n);
	return p;
}

char *
brdispl(
	bits displ,
	bits nbits)
{
	int sign, extended;

	sign = 1 << (nbits - 1);
	extended = (displ & sign ? displ - (sign << 1) : displ);
	db_disasm_symaddr = db_disasm_pc + (extended << 2);
	db_disasm_print_symaddr = TRUE;
	return hex(extended << 2);
}

char *
mbz(
	bits n)
{
	return n ? "[reserved bits not zero]" : "";
}

size_t db_disasm_string_size = 0;
#define DB_DISASM_STRING_MAXSIZE	4096
char db_disasm_string[DB_DISASM_STRING_MAXSIZE];

void *db_disasm_malloc(size_t size);	/* forward */
void *
db_disasm_malloc(
	size_t size)
{
	void * new_buf;

	if (db_disasm_string_size + size <= DB_DISASM_STRING_MAXSIZE) {
		new_buf = (void *) (db_disasm_string + db_disasm_string_size);
		db_disasm_string_size += size;
		return new_buf;
	}
	db_printf("db_disasm_malloc(size=%d) failed: %d left !\n",
		  size,
		  DB_DISASM_STRING_MAXSIZE - db_disasm_string_size);
	return (void *) 0;
}
