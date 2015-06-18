#ifndef SBITSET_H
#define SBITSET_H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * sbitset.h
 *
 * Bit set/array/vector handling.
 *
 * Copyright (c) 2015 F. Aragon. All rights reserved.
 */ 

#include "svector.h"

/*
 * Structures
 */

typedef sv_t sb_t;	/* "Hidden" structure (accessors are provided) */
			/* (bitset is implemented using a vector)  */

/*
 * Allocation
 */

#define SB_BITS2BYTES(n)	((n + 7) / 8)
#define sb_alloc(n)		sv_alloc_t(SV_U8, SB_BITS2BYTES(n))
#define sb_alloca(n)		sv_alloca_t(SV_U8, SB_BITS2BYTES(n))
#define sb_shrink(b)		sv_shrink(b)
#define sb_dup(b)		sv_dup(b)
#define sb_reset(b)		sv_reset(b)
#define sb_free			sv_free

/*
#API: |Allocate bitset (stack)|space preallocated to store n elements|bitset|O(1)|
sb_t *sb_alloca(const size_t initial_num_elems_reserve)

#API: |Allocate bitset (heap)|space preallocated to store n elements|bitset|O(1)|
sb_t *sb_alloc(const size_t initial_num_elems_reserve)

#API: |Free one or more bitsets (heap)|bitset; more bitsets (optional)|bitset|O(1)|
sb_t *sb_free(sb_t **b, ...)

#API: |Free unused space|bitset|same bitset (optional usage)|O(1)|
sb_t *sb_shrink(sb_t **c)

#API: |Duplicate bitset|bitset|output bitset|O(n)|
sb_t *sb_dup(const sb_t *src)

#API: |Reset bitset|bitset|output bitset|O(n)|
sb_t *sb_dup(const sb_t *src)
*/

/*
 * Accessors
 */

/* #API: |Get position of last bit set to 1 plus 1|bitset|Offset of last bit set to 1, plus 1|O(1)| */

static size_t sb_maxbitset(const sb_t *b)
{
	return b->aux;
}

/* #API: |Number of bits set to 1|bitset|Map number of elements|O(1)| */

static size_t sb_popcount(const sb_t *b)
{
	return b->aux2;
}

/*
 * Operations
 */

/* #API: |Access to nth bit|bitset; bit offset|1 or 0|O(1)| */

static int sb_test(const sb_t *b, const size_t nth)
{
	S_ASSERT(b);
	RETURN_IF(!b, 0);
	const size_t pos = nth / 8, mask = 1 << (nth % 8);
	RETURN_IF(pos >= b->aux, 0);
	const unsigned char *buf = (const unsigned char *)__sv_get_buffer_r(b);
	return (buf[pos] & mask) ? 1 : 0;
}

/* #API: |Set nth bit to 1|bitset; bit offset||O(1)| */

static void sb_set(sb_t **b, const size_t nth)
{
	S_ASSERT(!b);
	if (b) {
		const size_t pos = nth / 8, mask = 1 << (nth % 8);
		unsigned char *buf;
		if (pos >= (*b)->aux) {
			const size_t pinc = pos + 1 + (mask ? 1 : 0);
			if (sv_reserve(b, pinc) < pinc) {
				S_ERROR("not enough memory");
				return;
			}
			const size_t max = __sv_get_max_size(*b);
			buf = (unsigned char *)__sv_get_buffer(*b);
			memset(buf + (*b)->aux, 0, pinc - (*b)->aux);
			(*b)->aux = pinc;
		} else {
			buf = (unsigned char *)__sv_get_buffer(*b);
		}
		if ((buf[pos] & mask) == 0) {
			buf[pos] |= mask;
			(*b)->aux2++;
		}
	}
}

/* #API: |Set nth bit to 1|bitset; bit offset||O(1)| */

static void sb_clear(sb_t **b, const size_t nth)
{
	S_ASSERT(!b);
	if (b && *b) {
		const size_t pos = nth / 8;
		if (pos < (*b)->aux) {
			unsigned char *buf = (unsigned char *)__sv_get_buffer(*b);
			unsigned char prev = buf[pos];
			size_t mask = 1 << (nth % 8);
			if ((buf[pos] & mask) != 0) {
				buf[pos] &= ~mask;
				(*b)->aux2--;
			}
		}
		/* else: implicitly considered as set to 0 */
	}
}

#ifdef __cplusplus
}; /* extern "C" { */
#endif

#endif /* #ifndef SBITSET_H */

