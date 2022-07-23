/* ***** BEGIN LICENSE BLOCK *****
 * Version: RCSL 1.0/RPSL 1.0
 *
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved.
 *
 * The contents of this file, and the files included with this file, are
 * subject to the current version of the RealNetworks Public Source License
 * Version 1.0 (the "RPSL") available at
 * http://www.helixcommunity.org/content/rpsl unless you have licensed
 * the file under the RealNetworks Community Source License Version 1.0
 * (the "RCSL") available at http://www.helixcommunity.org/content/rcsl,
 * in which case the RCSL will apply. You may also obtain the license terms
 * directly from RealNetworks.  You may not use this file except in
 * compliance with the RPSL or, if you have a valid RCSL with RealNetworks
 * applicable to this file, the RCSL.  Please see the applicable RPSL or
 * RCSL for the rights, obligations and limitations governing use of the
 * contents of the file.
 *
 * This file is part of the Helix DNA Technology. RealNetworks is the
 * developer of the Original Code and owns the copyrights in the portions
 * it created.
 *
 * This file, and the files included with this file, is distributed and made
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Technology Compatibility Kit Test Suite(s) Location:
 *    http://www.helixcommunity.org/content/tck
 *
 * Contributor(s):
 *
 * ***** END LICENSE BLOCK ***** */

/**************************************************************************************
 * Fixed-point MP3 decoder
 * Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
 * June 2003
 *
 * assembly.h - assembly language functions and prototypes for supported platforms
 *
 * - inline rountines with access to 64-bit multiply results
 * - x86 (_WIN32) and ARM (ARM_ADS, _WIN32_WCE) versions included
 * - some inline functions are mix of asm and C for speed
 * - some functions are in native asm files, so only the prototype is given here
 *
 * MULSHIFT32(x, y)    signed multiply of two 32-bit integers (x and y), returns top 32 bits of 64-bit result
 * FASTABS(x)          branchless absolute value of signed integer x
 * CLZ(x)              count leading zeros in x
 * MADD32(sum, x, y)   sum [32-bit] += x [32-bit] * y [32-bit] >> 32
 * MSUB32(sum, x, y)   sum [32-bit] -= x [32-bit] * y [32-bit] >> 32
 * MADD64(sum, x, y)   sum [64-bit] += x [32-bit] * y [32-bit]
 * SAR64(sum, x, y)    64-bit right shift using __int64
 */

#ifndef _ASSEMBLY_H
#define _ASSEMBLY_H

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

/* ARM Cortex M3 and M4 */

/* 64-bit signed integer */
typedef long long int64_t;
/* 32-bit unsigned integer */
typedef unsigned int uint32_t;
/* 64-bit unsigned integer */
typedef unsigned long long uint64_t;

static __inline int FASTABS(int x)
{
	const int sign = x >> (sizeof(int) * 8 - 1);

	x ^= sign;
	x -= sign;

	return x;
}

static __inline int CLZ(int x)
{
	int result;

	__asm__ volatile (
		"CLZ %[result], %[value]"
		: [result] "=r" (result)
		: [value] "r" (x)
	);

	return result;
}

static __inline int64_t MADD64(int64_t sum, int x, int y)
{
	uint32_t hi = (uint32_t)(sum >> 32);
	uint32_t lo = (uint32_t)sum;

	/* SMLAL{<c>}  <RdLo>, <RdHi>, <Rn>, <Rm> */
	__asm__ volatile (
		"SMLAL %[lo], %[hi], %[a], %[b]"
		: [lo] "+r" (lo), [hi] "+r" (hi)
		: [a] "r" (x), [b] "r" (y)
	);

	return (int64_t)(lo | ((uint64_t)hi << 32));
}

#if defined(__ARM_ARCH_7EM__)

static __inline int MULSHIFT32(int x, int y)
{
	uint32_t result;

	/* SMMUL{R}<c>  <Rd>, <Rn>, <Rm> */
	__asm__ volatile (
		"SMMUL %[result], %[a], %[b]"
		: [result] "=r" (result)
		: [a] "r" (x), [b] "r" (y)
	);

	return (int)result;
}

static __inline int MADD32(int sum, int x, int y)
{
	uint32_t result;

	/* SMMLA{R}{<c>}{<q>}  <Rd>, <Rn>, <Rm>, <Ra> */
	__asm__ volatile (
		"SMMLA %[result], %[a], %[b], %[acc]"
		: [result] "=r" (result)
		: [a] "r" (x), [b] "r" (y), [acc] "r" (sum)
	);

	return (int)result;
}

static __inline int MSUB32(int sum, int x, int y)
{
	return (sum - MULSHIFT32(x, y));
}

#else

static __inline int MULSHIFT32(int x, int y)
{
	uint32_t hi;
	uint32_t lo;

	/* SMULL{<c>}{<q>}  <RdLo>, <RdHi>, <Rn>, <Rm> */
	__asm__ volatile (
		"SMULL %[lo], %[hi], %[a], %[b]"
		: [lo] "=r" (lo), [hi] "=r" (hi)
		: [a] "r" (x), [b] "r" (y)
	);

	return (int)hi;
}

static __inline int MADD32(int sum, int x, int y)
{
	return (sum + MULSHIFT32(x, y));
}

static __inline int MSUB32(int sum, int x, int y)
{
	return (sum - MULSHIFT32(x, y));
}

#endif

static __inline int64_t SAR64(int64_t x, int n)
{
	return (x >> n);
}

#else

/* 64-bit signed integer */
typedef long long int64_t;

static __inline int FASTABS(int x)
{
	return ((x > 0) ? x : -(x));
}

#if 0
static __inline int FASTABS(int x)
{
	int sign;

	sign = x >> (sizeof(int) * 8 - 1);
	x ^= sign;
	x -= sign;

	return x;
}
#endif

static __inline int CLZ(int x)
{
	int numZeros;

	if (!x)
		return (sizeof(int) * 8);

	numZeros = 0;
	while (!(x & 0x80000000)) {
		numZeros++;
		x <<= 1;
	}

	return numZeros;
}

static __inline int MADD32(int sum, int x, int y)
{
	const int64_t tmp = (int64_t)x * (int64_t)y;
	return (sum + (int)(tmp >> 32));
}

static __inline int MSUB32(int sum, int x, int y)
{
	const int64_t tmp = (int64_t)x * (int64_t)y;
	return (sum - (int)(tmp >> 32));
}

static __inline int64_t MADD64(int64_t sum, int x, int y)
{
	return (sum + (int64_t)x * (int64_t)y);
}

static __inline int MULSHIFT32(int x, int y)
{
	const int64_t tmp = (int64_t)x * (int64_t)y;
	return (tmp >> 32);
}

static __inline int64_t SAR64(int64_t x, int n)
{
	return (x >> n);
}

#endif

#endif /* _ASSEMBLY_H */
