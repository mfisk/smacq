#ifndef _SMACQ_IOVHASH_H
#define _SMACQ_IOVHASH_H

/* bhash.h: Jenkins hash support.
 *
 * Copyright (C) 1996 Bob Jenkins (bob_jenkins@burtleburtle.net)
 *
 * http://burtleburtle.net/bob/hash/
 *
 * These are the credits from Bob's sources:
 *
 * lookup2.c, by Bob Jenkins, December 1996, Public Domain.
 * hash(), hash2(), hash3, and mix() are externally useful functions.
 * Routines to test the hash are included if SELF_TEST is defined.
 * You can use this free for any purpose.  It has no warranty.
 *
 * Copyright (C) 2003 David S. Miller (davem@redhat.com)
 *
 * I've modified Bob's hash to be useful in the Linux kernel, and
 * any bugs present are surely my fault.  -DaveM
 *
 * Thanks to David for this creation, from a lesser mind.
 *
 * Copyright (C) 2003 Mike Fisk (mfisk@lanl.gov)
 *
 * Added iovec routines, use stdint.h, removed DaveM's additions, 
 * but kept his syntax. 
 *
 */

#include <stdint.h>

/* NOTE: Arguments are modified. */
#define __bhash_mix(a, b, c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

/* The golden ration: an arbitrary value */
#define BHASH_GOLDEN_RATIO	0x9e3779b9

/* The most generic version, hashes an arbitrary sequence
 * of bytes.  No alignment or length assumptions are made about
 * the input key.
 */
static inline uint32_t bhash(const unsigned char *key, const uint32_t length,
		uint32_t initval)
{
	uint32_t a, b, c, len;
	const unsigned char *k = key;

	len = length;
	a = b = BHASH_GOLDEN_RATIO;
	c = initval;

	while (len >= 12) {
		a += (k[0] +((uint32_t)k[1]<<8) +((uint32_t)k[2]<<16) +((uint32_t)k[3]<<24));
		b += (k[4] +((uint32_t)k[5]<<8) +((uint32_t)k[6]<<16) +((uint32_t)k[7]<<24));
		c += (k[8] +((uint32_t)k[9]<<8) +((uint32_t)k[10]<<16)+((uint32_t)k[11]<<24));

		__bhash_mix(a,b,c);

		k += 12;
		len -= 12;
	}

	c += length;
	switch (len) {
	case 11: c += ((uint32_t)k[10]<<24);
	case 10: c += ((uint32_t)k[9]<<16);
	case 9 : c += ((uint32_t)k[8]<<8);
	case 8 : b += ((uint32_t)k[7]<<24);
	case 7 : b += ((uint32_t)k[6]<<16);
	case 6 : b += ((uint32_t)k[5]<<8);
	case 5 : b += k[4];
	case 4 : a += ((uint32_t)k[3]<<24);
	case 3 : a += ((uint32_t)k[2]<<16);
	case 2 : a += ((uint32_t)k[1]<<8);
	case 1 : a += k[0];
	};

	__bhash_mix(a,b,c);

	return c;
}


#endif 
