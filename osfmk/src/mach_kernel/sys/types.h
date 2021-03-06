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
/* CMU_HIST */
/*
 * Revision 2.6  91/05/14  17:40:39  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:57:07  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:49:41  mrt]
 * 
 * Revision 2.4  90/08/27  22:13:03  dbg
 * 	Created.
 * 	[90/07/16            dbg]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon rights
 * to redistribute these changes.
 */
/*
 */
#ifndef	_SYS_TYPES_H_
#define	_SYS_TYPES_H_

#ifndef	ASSEMBLER

/*
 * Common type definitions that lots of old files seem to want.
 */

typedef	unsigned char	u_char;		/* unsigned char */
typedef	unsigned short	u_short;	/* unsigned short */
typedef	unsigned int	u_int;		/* unsigned int */
typedef	unsigned long	u_long;		/* unsigned long */

typedef struct _quad_ {
	unsigned int	val[2];		/* 2 32-bit values make... */
} quad;					/* an 8-byte item */

typedef	char *		caddr_t;	/* address of a (signed) char */

typedef int		time_t;		/* a signed 32    */
typedef unsigned int	daddr_t;	/* an unsigned 32 */
typedef	unsigned int	off_t;		/* another unsigned 32 */

typedef	unsigned short	dev_t;		/* another unsigned short */
#define	NODEV		((dev_t)-1)	/* and a null value for it */

#define	major(i)	(((i) >> 8) & 0xFF)
#define	minor(i)	((i) & 0xFF)
#define	makedev(i,j)	((((i) & 0xFF) << 8) | ((j) & 0xFF))

#define	NBBY		8

#ifndef	NULL
#define	NULL		((void *) 0)	/* the null pointer */
#endif

/*
 * Shorthand type definitions for unsigned storage classes
 */
typedef	unsigned char	uchar_t;
typedef	unsigned short	ushort_t;
typedef	unsigned int	uint_t;
typedef unsigned long	ulong_t;
typedef	volatile unsigned char	vuchar_t;
typedef	volatile unsigned short	vushort_t;
typedef	volatile unsigned int	vuint_t;
typedef volatile unsigned long	vulong_t;

/*
 * Shorthand type definitions for unsigned storage classes
 */
typedef	uchar_t		uchar;
typedef	ushort_t	ushort;
typedef	uint_t		uint;
typedef ulong_t		ulong;

#endif	/* !ASSEMBLER */

#endif	/* _SYS_TYPES_H_ */
