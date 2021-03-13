/*
	Copyright (C) 2014-2021 Igor van den Hoven ivdhoven@gmail.com
*/

/*
	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
	quadsort 1.1.3.5
*/

#ifndef QUADSORT_H
#define QUADSORT_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

typedef int CMPFUNC (const void *a, const void *b);

//#define cmp(a,b) (*(a) > *(b))

#define swap_two(array, swap)  \
{  \
	if (cmp(array, array + 1) > 0)  \
	{  \
		swap = array[1]; array[1] = array[0]; array[0] = swap;  \
	}  \
}

#define swap_three(array, swap)  \
{  \
	if (cmp(array, array + 1) > 0)  \
	{  \
		if (cmp(array, array + 2) <= 0)  \
		{  \
			swap = array[0]; array[0] = array[1]; array[1] = swap;  \
		}  \
		else if (cmp(array + 1, array + 2) > 0)  \
		{  \
			swap = array[0]; array[0] = array[2]; array[2] = swap;  \
		}  \
		else  \
		{  \
			swap = array[0]; array[0] = array[1]; array[1] = array[2]; array[2] = swap;  \
		}  \
	}  \
	else if (cmp(array + 1, array + 2) > 0)  \
	{  \
		if (cmp(array, array + 2) > 0)  \
		{  \
			swap = array[2]; array[2] = array[1]; array[1] = array[0]; array[0] = swap;  \
		}  \
		else   \
		{  \
			swap = array[2]; array[2] = array[1]; array[1] = swap;  \
		}  \
	}  \
}  \

#define swap_four(array, swap)  \
{  \
	if (cmp(array, array + 1) > 0)  \
	{  \
		swap = array[0]; array[0] = array[1]; array[1] = swap;  \
	}  \
	if (cmp(array + 2, array + 3) > 0)  \
	{  \
		swap = array[2]; array[2] = array[3]; array[3] = swap;  \
	}  \
	if (cmp(array + 1, array + 2) > 0)  \
	{  \
		if (cmp(array, array + 2) <= 0)  \
		{  \
			if (cmp(array + 1, array + 3) <= 0)  \
			{  \
				swap = array[1]; array[1] = array[2]; array[2] = swap;  \
			}  \
			else  \
			{  \
				swap = array[1]; array[1] = array[2]; array[2] = array[3]; array[3] = swap;  \
			}  \
		}  \
		else if (cmp(array, array + 3) > 0)  \
		{  \
			swap = array[1]; array[1] = array[3]; array[3] = swap;  \
			swap = array[0]; array[0] = array[2]; array[2] = swap;  \
		}  \
		else if (cmp(array + 1, array + 3) <= 0)  \
		{  \
			swap = array[1]; array[1] = array[0]; array[0] = array[2]; array[2] = swap;  \
		}  \
		else  \
		{  \
			swap = array[1]; array[1] = array[0]; array[0] = array[2]; array[2] = array[3]; array[3] = swap;  \
		}  \
	}  \
}

#define tail_swap_eight(array, pta, ptt, end, key, cmp) \
{ \
	pta = end++; \
	ptt = pta--; \
 \
	if (cmp(pta, ptt) > 0) \
	{ \
		key = *ptt; \
		*ptt-- = *pta--; \
 \
		if (cmp(pta - 2, &key) > 0) \
		{ \
			*ptt-- = *pta--; *ptt-- = *pta--; *ptt-- = *pta--; \
		} \
		if (pta > array && cmp(pta - 1, &key) > 0) \
		{ \
			*ptt-- = *pta--; *ptt-- = *pta--; \
		} \
		if (pta >= array && cmp(pta, &key) > 0) \
		{ \
			*ptt-- = *pta; \
		} \
		*ptt = key; \
	} \
}

#define swap_five(array, pta, ptt, end, key, cmp) \
{ \
	end = array + 4; \
 \
	pta = end++; \
	ptt = pta--; \
 \
	if (cmp(pta, ptt) > 0) \
	{ \
		key = *ptt; \
		*ptt-- = *pta--; \
 \
		if (pta > array && cmp(pta - 1, &key) > 0) \
		{ \
			*ptt-- = *pta--; *ptt-- = *pta--; \
		} \
		if (pta >= array && cmp(pta, &key) > 0) \
		{ \
			*ptt-- = *pta; \
		} \
		*ptt = key; \
	} \
}

#define swap_six(array, pta, ptt, end, key, cmp) \
{ \
	swap_five(array, pta, ptt, end, key, cmp); \
	tail_swap_eight(array, pta, ptt, end, key, cmp); \
}

#define swap_seven(array, pta, ptt, end, key, cmp) \
{ \
	swap_six(array, pta, ptt, end, key, cmp); \
	tail_swap_eight(array, pta, ptt, end, key, cmp); \
}

#define swap_eight(array, pta, ptt, end, key, cmp) \
{ \
	swap_seven(array, pta, ptt, end, key, cmp); \
	tail_swap_eight(array, pta, ptt, end, key, cmp); \
}

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

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR char
#define FUNC(NAME) NAME##8
#define STRUCT(NAME) struct NAME##8

#include "quadsort.c"

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

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR short
#define FUNC(NAME) NAME##16
#define STRUCT(NAME) struct NAME##16

#include "quadsort.c"

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

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR int
#define FUNC(NAME) NAME##32
#define STRUCT(NAME) struct NAME##32

#include "quadsort.c"

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

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR long long
#define FUNC(NAME) NAME##64
#define STRUCT(NAME) struct NAME##64

#include "quadsort.c"

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

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR long double
#define FUNC(NAME) NAME##128
#define STRUCT(NAME) struct NAME##128

#include "quadsort.c"


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
			return quadsort8(array, nmemb, cmp);

		case sizeof(short):
			return quadsort16(array, nmemb, cmp);

		case sizeof(int):
			return quadsort32(array, nmemb, cmp);

		case sizeof(long long):
			return quadsort64(array, nmemb, cmp);

		case sizeof(long double):
			return quadsort128(array, nmemb, cmp);

		default:
			return assert(size == sizeof(char) || size == sizeof(short) || size == sizeof(int) || size == sizeof(long long) || size == sizeof(long double));
	}
}

#undef VAR
#undef FUNC
#undef STRUCT

#endif
