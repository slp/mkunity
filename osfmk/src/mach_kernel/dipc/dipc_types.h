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
 * 
 */
/*
 * MkLinux
 */


/*
 *	File:	dipc/dipc_types.h
 *	Author:	Alan Langerman
 *	Date:	1994
 *
 *	Fundamental types for the DIPC implementation.
 */

#ifndef	_DIPC_DIPC_TYPES_H_
#define	_DIPC_DIPC_TYPES_H_

#include <mach/boolean.h>
#include <mach/kkt_types.h>

/*
 *	Error types passed within DIPC and back to callers
 *	of DIPC are contained in a dipc_return_t.  Definitions
 *	for the values contained in this type may be found
 *	in dipc_error.h.
 */
typedef unsigned int	dipc_return_t;

/*
 *	A port_id uniquely specifies a port on a given node.
 *	Port id's are presumed never to repeat.
 */
typedef unsigned int	port_id;	/* at least 32 bits */
#define	PORT_ID_NULL	((port_id) -1)

/*
 *	The UID is the network-wide name for a port.  It is a
 *	64-bit quantity consisting of a node_name and a port_id.
 *	A UID is constructed for a port whenever an associated
 *	right must become accessible by a remote node.  This UID
 *	remains fixed during the lifetime of the port, even if
 *	all rights for the port migrate away from the originating
 *	node.  Thus, it is possible for the "origin" node_name within
 *	the UID to not match the node where the receive right for
 *	the port currently resides.
 *
 *	All UIDs generated by all nodes within a NORMA domain have
 *	the same format and size.
 *
 *	An identifier value of zero is reserved, and means that
 *	the UID is invalid/unassigned.
 */
typedef struct uid {
	node_name	origin;		/* node that constructed the uid */
	port_id		identifier;	/* unique, node-relative id */
} uid_t;

/*
 *	Compare two UIDs for equality.  Check identifier first, as
 *	it will often eliminate the need to check the origin.
 */
#define	DIPC_UID_EQUAL(u1, u2)	(((u1)->identifier == (u2)->identifier) &&\
				 ((u1)->origin == (u2)->origin))

/*
 *	Use this macro to build a UID, e.g.,
 *		uid_t	uid;
 *		DIPC_UID_MAKE(dipc_node_self(), (port_id) unique_identifier);
 */
#define	DIPC_UID_MAKE(uid, node, ident)				\
	((uid)->origin = (node), (uid)->identifier = (ident))

/*
 *	UID representations for null and dead ports.
 *	The node name is NODE_NAME_NULL (an illegal node name)
 *	and the identifier specifies which kind of port.
 */
#define	UID_NULL_PORT		1
#define	UID_DEAD_PORT		2

#define	DIPC_UID_NULL(uidp)						\
	((uidp)->origin = NODE_NAME_NULL, (uidp)->identifier = UID_NULL_PORT)

#define	DIPC_IS_UID_NULL(uid)						\
	((uid)->identifier == UID_NULL_PORT && (uid)->origin == NODE_NAME_NULL)

#define	DIPC_UID_DEAD(uidp)						\
	((uidp)->origin = NODE_NAME_NULL, (uidp)->identifier = UID_DEAD_PORT)

#define	DIPC_IS_UID_DEAD(uid)						\
	((uid)->identifier == UID_DEAD_PORT && (uid)->origin == NODE_NAME_NULL)

/*
 *	For assertion checking.
 */
#define	DIPC_UID_VALID(uid)						\
	(dipc_node_is_valid((uid)->origin) && (uid)->identifier != PORT_ID_NULL)

#endif	/* _DIPC_DIPC_TYPES_H_ */
