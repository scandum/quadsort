// quadsort 1.2.1.3 - Igor van den Hoven ivdhoven@gmail.com

#ifndef QUADSORT_H
#define QUADSORT_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <float.h>
#include <string.h>

//#include <stdalign.h>

typedef int CMPFUNC (const void *a, const void *b);

//#define cmp(a,b) (*(a) > *(b))


// When sorting an array of pointers, like a string array, the QUAD_CACHE needs
// to be set for proper performance when sorting large arrays.
// quadsort_prim() can be used to sort arrays of 32 and 64 bit integers
// without a comparison function or cache restrictions.

// With a 6 MB L3 cache a value of 262144 works well.

#ifdef cmp
  #define QUAD_CACHE 4294967295
#else
//#define QUAD_CACHE 131072
  #define QUAD_CACHE 262144
//#define QUAD_CACHE 524288
//#define QUAD_CACHE 4294967295
#endif

// utilize branchless ternary operations in clang

#if !defined __clang__
#define head_branchless_merge(ptd, x, ptl, ptr, cmp)  \
	x = cmp(ptl, ptr) <= 0;  \
	*ptd = *ptl;  \
	ptl += x;  \
	ptd[x] = *ptr;  \
	ptr += !x;  \
	ptd++;
#else
#define head_branchless_merge(ptd, x, ptl, ptr, cmp)  \
	*ptd++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
#endif

#if !defined __clang__
#define tail_branchless_merge(tpd, y, tpl, tpr, cmp)  \
	y = cmp(tpl, tpr) <= 0;  \
	*tpd = *tpl;  \
	tpl -= !y;  \
	tpd--;  \
	tpd[y] = *tpr;  \
	tpr -= y;
#else
#define tail_branchless_merge(tpd, x, tpl, tpr, cmp)  \
	*tpd-- = cmp(tpl, tpr) > 0 ? *tpl-- : *tpr--;
#endif

// guarantee small parity merges are inlined with minimal overhead

#define parity_merge_two(array, swap, x, ptl, ptr, pts, cmp)  \
	ptl = array; ptr = array + 2; pts = swap;  \
	head_branchless_merge(pts, x, ptl, ptr, cmp);  \
	*pts = cmp(ptl, ptr) <= 0 ? *ptl : *ptr;  \
  \
	ptl = array + 1; ptr = array + 3; pts = swap + 3;  \
	tail_branchless_merge(pts, x, ptl, ptr, cmp);  \
	*pts = cmp(ptl, ptr)  > 0 ? *ptl : *ptr;

#define parity_merge_four(array, swap, x, ptl, ptr, pts, cmp)  \
	ptl = array + 0; ptr = array + 4; pts = swap;  \
	head_branchless_merge(pts, x, ptl, ptr, cmp);  \
	head_branchless_merge(pts, x, ptl, ptr, cmp);  \
	head_branchless_merge(pts, x, ptl, ptr, cmp);  \
	*pts = cmp(ptl, ptr) <= 0 ? *ptl : *ptr;  \
  \
	ptl = array + 3; ptr = array + 7; pts = swap + 7;  \
	tail_branchless_merge(pts, x, ptl, ptr, cmp);  \
	tail_branchless_merge(pts, x, ptl, ptr, cmp);  \
	tail_branchless_merge(pts, x, ptl, ptr, cmp);  \
	*pts = cmp(ptl, ptr)  > 0 ? *ptl : *ptr;


#if !defined __clang__
#define branchless_swap(pta, swap, x, cmp)  \
	x = cmp(pta, pta + 1) > 0;  \
	swap = pta[!x];  \
	pta[0] = pta[x];  \
	pta[1] = swap;
#else
#define branchless_swap(pta, swap, x, cmp)  \
	x = 0;  \
	swap = cmp(pta, pta + 1) > 0 ? pta[x++] : pta[1];  \
	pta[0] = pta[x];  \
	pta[1] = swap;
#endif

#define swap_branchless(pta, swap, x, y, cmp)  \
	x = cmp(pta, pta + 1) > 0;  \
	y = !x;  \
	swap = pta[y];  \
	pta[0] = pta[x];  \
	pta[1] = swap;

//////////////////////////////////////////////////////////
// ┌───────────────────────────────────────────────────┐//
// │       ██████┐ ██████┐    ██████┐ ██████┐████████┐ │//
// │       └────██┐└────██┐   ██┌──██┐└─██┌─┘└──██┌──┘ │//
// │        █████┌┘ █████┌┘   ██████┌┘  ██│     ██│    │//
// │        └───██┐██┌───┘    ██┌──██┐  ██│     ██│    │//
// │       ██████┌┘███████┐   ██████┌┘██████┐   ██│    │//
// │       └─────┘ └──────┘   └─────┘ └─────┘   └─┘    │//
// └───────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#define VAR int
#define FUNC(NAME) NAME##32

#include "quadsort.c"

#undef VAR
#undef FUNC

// quadsort_prim

#define VAR int
#define FUNC(NAME) NAME##_int32
#ifndef cmp
  #define cmp(a,b) (*(a) > *(b))
  #include "quadsort.c"
  #undef cmp
#else
  #include "quadsort.c"
#endif
#undef VAR
#undef FUNC

#define VAR unsigned int
#define FUNC(NAME) NAME##_uint32
#ifndef cmp
  #define cmp(a,b) (*(a) > *(b))
  #include "quadsort.c"
  #undef cmp
#else
  #include "quadsort.c"
#endif
#undef VAR
#undef FUNC

//////////////////////////////////////////////////////////
// ┌───────────────────────────────────────────────────┐//
// │        █████┐ ██┐  ██┐   ██████┐ ██████┐████████┐ │//
// │       ██┌───┘ ██│  ██│   ██┌──██┐└─██┌─┘└──██┌──┘ │//
// │       ██████┐ ███████│   ██████┌┘  ██│     ██│    │//
// │       ██┌──██┐└────██│   ██┌──██┐  ██│     ██│    │//
// │       └█████┌┘     ██│   ██████┌┘██████┐   ██│    │//
// │        └────┘      └─┘   └─────┘ └─────┘   └─┘    │//
// └───────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#define VAR long long
#define FUNC(NAME) NAME##64

#include "quadsort.c"

#undef VAR
#undef FUNC

// quadsort_prim

#define VAR long long
#define FUNC(NAME) NAME##_int64
#ifndef cmp
  #define cmp(a,b) (*(a) > *(b))
  #include "quadsort.c"
  #undef cmp
#else
  #include "quadsort.c"
#endif
#undef VAR
#undef FUNC

#define VAR unsigned long long
#define FUNC(NAME) NAME##_uint64
#ifndef cmp
  #define cmp(a,b) (*(a) > *(b))
  #include "quadsort.c"
  #undef cmp
#else
  #include "quadsort.c"
#endif
#undef VAR
#undef FUNC

// This section is outside of 32/64 bit pointer territory, so no cache checks
// necessary, unless sorting 32+ byte structures.

#undef QUAD_CACHE
#define QUAD_CACHE 4294967295

//////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────┐//
//│                █████┐    ██████┐ ██████┐████████┐  │//
//│               ██┌──██┐   ██┌──██┐└─██┌─┘└──██┌──┘  │//
//│               └█████┌┘   ██████┌┘  ██│     ██│     │//
//│               ██┌──██┐   ██┌──██┐  ██│     ██│     │//
//│               └█████┌┘   ██████┌┘██████┐   ██│     │//
//│                └────┘    └─────┘ └─────┘   └─┘     │//
//└────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#define VAR char
#define FUNC(NAME) NAME##8

#include "quadsort.c"

#undef VAR
#undef FUNC

//////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────┐//
//│           ▄██┐   █████┐    ██████┐ ██████┐████████┐│//
//│          ████│  ██┌───┘    ██┌──██┐└─██┌─┘└──██┌──┘│//
//│          └─██│  ██████┐    ██████┌┘  ██│     ██│   │//
//│            ██│  ██┌──██┐   ██┌──██┐  ██│     ██│   │//
//│          ██████┐└█████┌┘   ██████┌┘██████┐   ██│   │//
//│          └─────┘ └────┘    └─────┘ └─────┘   └─┘   │//
//└────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#define VAR short
#define FUNC(NAME) NAME##16

#include "quadsort.c"

#undef VAR
#undef FUNC

//////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────┐//
//│  ▄██┐  ██████┐  █████┐    ██████┐ ██████┐████████┐ │//
//│ ████│  └────██┐██┌──██┐   ██┌──██┐└─██┌─┘└──██┌──┘ │//
//│ └─██│   █████┌┘└█████┌┘   ██████┌┘  ██│     ██│    │//
//│   ██│  ██┌───┘ ██┌──██┐   ██┌──██┐  ██│     ██│    │//
//│ ██████┐███████┐└█████┌┘   ██████┌┘██████┐   ██│    │//
//│ └─────┘└──────┘ └────┘    └─────┘ └─────┘   └─┘    │//
//└────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

// 128 reflects the name, though the actual size of a long double is 64, 80,
// 96, or 128 bits, depending on platform.

#if (DBL_MANT_DIG < LDBL_MANT_DIG)
  #define VAR long double
  #define FUNC(NAME) NAME##128
  #include "quadsort.c"
  #undef VAR
  #undef FUNC
#endif

///////////////////////////////////////////////////////////
//┌─────────────────────────────────────────────────────┐//
//│ ██████┐██┐   ██┐███████┐████████┐ ██████┐ ███┐  ███┐│//
//│██┌────┘██│   ██│██┌────┘└──██┌──┘██┌───██┐████┐████││//
//│██│     ██│   ██│███████┐   ██│   ██│   ██│██┌███┌██││//
//│██│     ██│   ██│└────██│   ██│   ██│   ██│██│└█┌┘██││//
//│└██████┐└██████┌┘███████│   ██│   └██████┌┘██│ └┘ ██││//
//│ └─────┘ └─────┘ └──────┘   └─┘    └─────┘ └─┘    └─┘│//
//└─────────────────────────────────────────────────────┘//
///////////////////////////////////////////////////////////

/*
typedef struct {char bytes[32];} struct256;
#define VAR struct256
#define FUNC(NAME) NAME##256

#include "quadsort.c"

#undef VAR
#undef FUNC
*/

///////////////////////////////////////////////////////////////////////////////
//┌─────────────────────────────────────────────────────────────────────────┐//
//│    ██████┐ ██┐   ██┐ █████┐ ██████┐ ███████┐ ██████┐ ██████┐ ████████┐  │//
//│   ██┌───██┐██│   ██│██┌──██┐██┌──██┐██┌────┘██┌───██┐██┌──██┐└──██┌──┘  │//
//│   ██│   ██│██│   ██│███████│██│  ██│███████┐██│   ██│██████┌┘   ██│     │//
//│   ██│▄▄ ██│██│   ██│██┌──██│██│  ██│└────██│██│   ██│██┌──██┐   ██│     │//
//│   └██████┌┘└██████┌┘██│  ██│██████┌┘███████│└██████┌┘██│  ██│   ██│     │//
//│    └──▀▀─┘  └─────┘ └─┘  └─┘└─────┘ └──────┘ └─────┘ └─┘  └─┘   └─┘     │//
//└─────────────────────────────────────────────────────────────────────────┘//
///////////////////////////////////////////////////////////////////////////////


void quadsort(void *array, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	if (nmemb < 2)
	{
		return;
	}

	switch (size)
	{
		case sizeof(char):
			quadsort8(array, nmemb, cmp);
			return;

		case sizeof(short):
			quadsort16(array, nmemb, cmp);
			return;

		case sizeof(int):
			quadsort32(array, nmemb, cmp);
			return;

		case sizeof(long long):
			quadsort64(array, nmemb, cmp);
			return;
#if (DBL_MANT_DIG < LDBL_MANT_DIG)
		case sizeof(long double):
			quadsort128(array, nmemb, cmp);
			return;
#endif
//		case sizeof(struct256):
//			quadsort256(array, nmemb, cmp);
//			return;

		default:
#if (DBL_MANT_DIG < LDBL_MANT_DIG)
			assert(size == sizeof(char) || size == sizeof(short) || size == sizeof(int) || size == sizeof(long long) || size == sizeof(long double));
#else
			assert(size == sizeof(char) || size == sizeof(short) || size == sizeof(int) || size == sizeof(long long));
#endif
//			qsort(array, nmemb, size, cmp);
	}
}

// suggested size values for primitives:

//		case  0: unsigned char
//		case  1: signed char
//		case  2: signed short
//		case  3: unsigned short
//		case  4: signed int
//		case  5: unsigned int
//		case  6: float
//		case  7: double
//		case  8: signed long long
//		case  9: unsigned long long
//		case  ?: long double, use sizeof(long double):

void quadsort_prim(void *array, size_t nmemb, size_t size)
{
	if (nmemb < 2)
	{
		return;
	}

	switch (size)
	{
		case 4:
			quadsort_int32(array, nmemb, NULL);
			return;
		case 5:
			quadsort_uint32(array, nmemb, NULL);
			return;
		case 8:
			quadsort_int64(array, nmemb, NULL);
			return;
		case 9:
			quadsort_uint64(array, nmemb, NULL);
			return;
		default:
			assert(size == sizeof(int) || size == sizeof(int) + 1 || size == sizeof(long long) || size == sizeof(long long) + 1);
			return;
	}
}

// Sort arrays of structures, the comparison function must be by reference.

void quadsort_size(void *array, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	char **pti, *pta, *pts;
	size_t index, offset;

	if (nmemb < 2)
	{
		return;
	}
	pta = (char *) array;
	pti = (char **) malloc(nmemb * sizeof(char *));

	assert(pti != NULL);

	for (index = offset = 0 ; index < nmemb ; index++)
	{
		pti[index] = pta + offset;

		offset += size;
	}

	switch (sizeof(size_t))
	{
		case 4: quadsort32(pti, nmemb, cmp); break;
		case 8: quadsort64(pti, nmemb, cmp); break;
	}

	pts = (char *) malloc(nmemb * size);

	assert(pts != NULL);
	
	for (index = 0 ; index < nmemb ; index++)
	{
		memcpy(pts, pti[index], size);

		pts += size;
	}
	pts -= nmemb * size;

	memcpy(array, pts, nmemb * size);

	free(pti);
	free(pts);
}

#undef QUAD_CACHE

#endif
