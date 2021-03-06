#ifndef _IF_ENC_H_
#define _IF_ENC_H_
/*
    Copyright � 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
*/

/*
 * The authors of this code are John Ioannidis (ji@tla.org),
 * Angelos D. Keromytis (kermit@csd.uch.gr) and 
 * Niels Provos (provos@physnet.uni-hamburg.de).
 *
 * This code was written by John Ioannidis for BSD/OS in Athens, Greece, 
 * in November 1995.
 *
 * Ported to OpenBSD and NetBSD, with additional transforms, in December 1996,
 * by Angelos D. Keromytis.
 *
 * Additional transforms and features in 1997 and 1998 by Angelos D. Keromytis
 * and Niels Provos.
 *
 * Copyright (C) 1995, 1996, 1997, 1998 by John Ioannidis, Angelos D. Keromytis
 * and Niels Provos.
 *	
 * Permission to use, copy, and modify this software without fee
 * is hereby granted, provided that this entire notice is included in
 * all copies of any software which is or includes a copy or
 * modification of this software. 
 * You may use this code under the GNU public license if you so wish. Please
 * contribute changes back to the authors under this freer than GPL license
 * so that we may further the use of strong encryption without limitations to
 * all.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTY. IN PARTICULAR, NONE OF THE AUTHORS MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE
 * MERCHANTABILITY OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR
 * PURPOSE.
 */

#define ENCMTU		(1024+512)
#define ENC_HDRLEN	12

struct enc_softc {
	struct ifnet		sc_if;  /* the interface */
	union sockaddr_union	sc_dst;
	u_int32_t		sc_spi;
	u_int32_t		sc_sproto;
};

struct ifsa {
	char			sa_ifname[IFNAMSIZ];	/* bridge ifs name */
	u_int32_t		sa_spi;
	u_int8_t		sa_proto;
	union sockaddr_union	sa_dst;
};

struct enchdr {
	u_int32_t af;
	u_int32_t spi;
	u_int32_t flags;
};

extern struct enc_softc encif[];
#endif
