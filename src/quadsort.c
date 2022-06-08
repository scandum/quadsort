/*
	Copyright (C) 2014-2022 Igor van den Hoven ivdhoven@gmail.com
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
	quadsort 1.1.5.3
*/

// the next seven functions are used for sorting 0 to 31 elements

void FUNC(unguarded_insert)(VAR *array, size_t offset, size_t nmemb, CMPFUNC *cmp)
{
	VAR key, *pta, *end;
	size_t i, top, x, y;

	for (i = offset ; i < nmemb ; i++)
	{
		pta = end = array + i;

		if (cmp(--pta, end) <= 0)
		{
			continue;
		}

		key = *end;

		if (cmp(array + 1, &key) > 0)
		{
			top = i - 1;

			do
			{
				*end-- = *pta--;
			}
			while (--top);

			*end-- = key;
		}
		else
		{
			do
			{
				*end-- = *pta--;
				*end-- = *pta--;
			}
			while (cmp(pta, &key) > 0);

			end[0] = end[1];
			end[1] = key;
		}
		x = cmp(end, end + 1) > 0; y = !x; key = end[y]; end[0] = end[x]; end[1] = key;
	}
}

void FUNC(bubble_sort)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	VAR swap, *pta;
	size_t x, y;

	if (nmemb > 1)
	{
		pta = array;

		if (nmemb > 2)
		{
			x = cmp(pta, pta + 1) > 0; y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta++;
			x = cmp(pta, pta + 1) > 0; y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta--;
		}
		x = cmp(pta, pta + 1) > 0; y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap;
	}
}

void FUNC(quad_swap_four)(VAR *array, CMPFUNC *cmp)
{
	VAR *pta, swap;
	size_t x, y;

	pta = array;
	x = cmp(pta, pta + 1) > 0; y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta += 2;
	x = cmp(pta, pta + 1) > 0; y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta--;

	if (cmp(pta, pta + 1) > 0)
	{
		swap = pta[0]; pta[0] = pta[1]; pta[1] = swap; pta--;

		x = cmp(pta, pta + 1) > 0; y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta += 2;
		x = cmp(pta, pta + 1) > 0; y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta--;
		x = cmp(pta, pta + 1) > 0; y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap;
	}
}

void FUNC(parity_swap_eight)(VAR *array, CMPFUNC *cmp)
{
	VAR swap[8], *ptl, *ptr, *pts;
	unsigned char x, y;

	ptl = array;
	x = cmp(ptl, ptl + 1) > 0; y = !x; swap[0] = ptl[y]; ptl[0] = ptl[x]; ptl[1] = swap[0]; ptl += 2;
	x = cmp(ptl, ptl + 1) > 0; y = !x; swap[0] = ptl[y]; ptl[0] = ptl[x]; ptl[1] = swap[0]; ptl += 2;
	x = cmp(ptl, ptl + 1) > 0; y = !x; swap[0] = ptl[y]; ptl[0] = ptl[x]; ptl[1] = swap[0]; ptl += 2;
	x = cmp(ptl, ptl + 1) > 0; y = !x; swap[0] = ptl[y]; ptl[0] = ptl[x]; ptl[1] = swap[0];

	if (cmp(array + 1, array + 2) <= 0 && cmp(array + 3, array + 4) <= 0 && cmp(array + 5, array + 6) <= 0)
	{
		return;
	}
	parity_merge_two(array + 0, swap + 0, x, y, ptl, ptr, pts, cmp);
	parity_merge_two(array + 4, swap + 4, x, y, ptl, ptr, pts, cmp);

	parity_merge_four(swap, array, x, y, ptl, ptr, pts, cmp);
}

void FUNC(parity_merge)(VAR *dest, VAR *from, size_t block, size_t nmemb, CMPFUNC *cmp)
{
	VAR *ptl, *ptr, *tpl, *tpr, *tpd, *ptd;
	unsigned char x, y;

	ptl = from;
	ptr = from + block;
	ptd = dest;
	tpl = from + block - 1;
	tpr = from + nmemb - 1;
	tpd = dest + nmemb - 1;

	while (--block)
	{
		x = cmp(ptl, ptr) <= 0; y = !x; ptd[x] = *ptr; ptr += y; ptd[y] = *ptl; ptl += x; ptd++;
		x = cmp(tpl, tpr) <= 0; y = !x; tpd--; tpd[x] = *tpr; tpr -= x; tpd[y] = *tpl; tpl -= y;
	}
	*ptd = cmp(ptl, ptr) <= 0 ? *ptl : *ptr;
	*tpd = cmp(tpl, tpr)  > 0 ? *tpl : *tpr;
}

void FUNC(parity_swap_sixteen)(VAR *array, CMPFUNC *cmp)
{
	VAR swap[16], *ptl, *ptr, *pts;
	unsigned char x, y;

	FUNC(quad_swap_four)(array +  0, cmp);
	FUNC(quad_swap_four)(array +  4, cmp);
	FUNC(quad_swap_four)(array +  8, cmp);
	FUNC(quad_swap_four)(array + 12, cmp);

	if (cmp(array + 3, array + 4) <= 0 && cmp(array + 7, array + 8) <= 0 && cmp(array + 11, array + 12) <= 0)
	{
		return;
	}
	parity_merge_four(array + 0, swap + 0, x, y, ptl, ptr, pts, cmp);
	parity_merge_four(array + 8, swap + 8, x, y, ptl, ptr, pts, cmp);

	FUNC(parity_merge)(array, swap, 8, 16, cmp);
}

void FUNC(tail_swap)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb < 4)
	{
		FUNC(bubble_sort)(array, nmemb, cmp);
		return;
	}
	if (nmemb < 8)
	{
		FUNC(quad_swap_four)(array, cmp);
		FUNC(unguarded_insert)(array, 4, nmemb, cmp);
		return;
	}
	if (nmemb < 16)
	{
		FUNC(parity_swap_eight)(array, cmp);
		FUNC(unguarded_insert)(array, 8, nmemb, cmp);
		return;
	}
	FUNC(parity_swap_sixteen)(array, cmp);
	FUNC(unguarded_insert)(array, 16, nmemb, cmp);
}

// the next three functions create sorted blocks of 32 elements

void FUNC(parity_tail_swap_eight)(VAR *array, CMPFUNC *cmp)
{
	VAR swap[8], *ptl, *ptr, *pts;
	unsigned char x, y;

	if (cmp(array + 4, array + 5) > 0) { swap[5] = array[4]; array[4] = array[5]; array[5] = swap[5]; }
	if (cmp(array + 6, array + 7) > 0) { swap[7] = array[6]; array[6] = array[7]; array[7] = swap[7]; } else

	if (cmp(array + 3, array + 4) <= 0 && cmp(array + 5, array + 6) <= 0)
	{
		return;
	}
	swap[0] = array[0]; swap[1] = array[1]; swap[2] = array[2]; swap[3] = array[3];

	parity_merge_two(array + 4, swap + 4, x, y, ptl, ptr, pts, cmp);

	parity_merge_four(swap, array, x, y, ptl, ptr, pts, cmp);
}

void FUNC(parity_tail_flip_eight)(VAR *array, CMPFUNC *cmp)
{
	VAR swap[8], *ptl, *ptr, *pts;
	unsigned char x, y;

	if (cmp(array + 3, array + 4) <= 0)
	{
		return;
	}
	swap[0] = array[0]; swap[1] = array[1]; swap[2] = array[2]; swap[3] = array[3];
	swap[4] = array[4]; swap[5] = array[5]; swap[6] = array[6]; swap[7] = array[7];

	parity_merge_four(swap, array, x, y, ptl, ptr, pts, cmp);
}

void FUNC(tail_merge)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t block, CMPFUNC *cmp);

size_t FUNC(quad_swap)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	VAR swap[32];
	size_t count, reverse, x, y;
	VAR *pta, *pts, *pte, tmp;

	pta = array;

	count = nmemb / 8 * 2;

	while (count--)
	{
		switch ((cmp(pta, pta + 1) > 0) | (cmp(pta + 1, pta + 2) > 0) * 2 | (cmp(pta + 2, pta + 3) > 0) * 4)
		{
			case 0:
				break;
			case 1:
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
				pta += 1; x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				pta += 1; x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				pta -= 2;
				break;
			case 2:
				tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				          x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				pta += 2; x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				pta -= 1; x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				pta -= 1;
				break;
			case 3:
				tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
				pta += 2; x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				pta -= 1; x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				pta -= 1;
				break;
			case 4:
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
				pta += 1; x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				pta -= 1; x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				break;
			case 5:
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
				pta += 1; x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				pta += 1; x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				pta -= 2; x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				break;
			case 6:
				tmp = pta[1]; pta[1] = pta[3]; pta[3] = tmp;
				          x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				pta += 1; x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp;
				pta -= 1;
				break;
			case 7:
				pts = pta;
				goto swapper;
		}
		count--;

		FUNC(parity_tail_swap_eight)(pta, cmp);

		pta += 8;

		continue;

		swapper:

		pta += 4;

		if (count--)
		{
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				if (cmp(&pta[2], &pta[3]) > 0)
				{
					if (cmp(&pta[1], &pta[2]) > 0)
					{
						if (cmp(&pta[-1], &pta[0]) > 0)
						{
							goto swapper;
						}
					}
					tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
				}
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}
			else if (cmp(&pta[2], &pta[3]) > 0)
			{
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
			}

			if (cmp(&pta[1], &pta[2]) > 0)
			{
				tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;

				x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp; pta += 2;
				x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp; pta -= 1;
				x = cmp(pta, pta + 1) > 0; y = !x; tmp = pta[y]; pta[0] = pta[x]; pta[1] = tmp; pta -= 1;
			}
			pte = pta - 1;

			reverse = (pte - pts) / 2;

			do
			{
				tmp = *pts; *pts++ = *pte; *pte-- = tmp;
			}
			while (reverse--);

			if (count % 2 == 0)
			{
				pta -= 4;

				FUNC(parity_tail_flip_eight)(pta, cmp);
			}
			else
			{
				count--;

				FUNC(parity_tail_swap_eight)(pta, cmp);
			}
			pta += 8;

			continue;
		}

		if (pts == array)
		{
			switch (nmemb % 8)
			{
				case 7: if (cmp(pta + 5, pta + 6) <= 0) break;
				case 6: if (cmp(pta + 4, pta + 5) <= 0) break;
				case 5: if (cmp(pta + 3, pta + 4) <= 0) break;
				case 4: if (cmp(pta + 2, pta + 3) <= 0) break;
				case 3: if (cmp(pta + 1, pta + 2) <= 0) break;
				case 2: if (cmp(pta + 0, pta + 1) <= 0) break;
				case 1: if (cmp(pta - 1, pta + 0) <= 0) break;
				case 0:
					pte = pts + nmemb - 1;

					reverse = (pte - pts) / 2;

					do
					{
						tmp = *pts; *pts++ = *pte; *pte-- = tmp;
					}
					while (reverse--);

					return 1;
			}
		}
		pte = pta - 1;

		reverse = (pte - pts) / 2;

		do
		{
			tmp = *pts; *pts++ = *pte; *pte-- = tmp;
		}
		while (reverse--);

		break;
	}

	FUNC(tail_swap)(pta, nmemb % 8, cmp);

	pta = array;

	for (count = nmemb / 32 ; count-- ; pta += 32)
	{
		if (cmp(pta + 7, pta + 8) <= 0 && cmp(pta + 15, pta + 16) <= 0 && cmp(pta + 23, pta + 24) <= 0)
		{
			continue;
		}
		FUNC(parity_merge)(swap, pta, 8, 16, cmp);
		FUNC(parity_merge)(swap + 16, pta + 16, 8, 16, cmp);
		FUNC(parity_merge)(pta, swap, 16, 32, cmp);
	}

	if (nmemb % 32 > 8)
	{
		FUNC(tail_merge)(pta, swap, 32, nmemb % 32, 8, cmp);
	}
	return 0;
}

// quad merge support routines

void FUNC(galloping_merge)(VAR *dest, VAR *from, size_t block, CMPFUNC *cmp)
{
	VAR *ptl, *tpl, *ptr, *tpr, *ptd, *tpd;
	size_t loop, x, y;

	ptl = from;
	ptr = from + block;
	tpl = ptr - 1;
	tpr = ptr + block - 1;

	if (cmp(ptl + 15, ptr) > 0 && cmp(ptl, ptr + 15) <= 0 && cmp(tpl, tpr - 15) > 0 && cmp(tpl - 15, tpr) <= 0)
	{
		return FUNC(parity_merge)(dest, from, block, block * 2, cmp);
	}

	ptd = dest;
	tpd = dest + 2 * block - 1;

	while (tpl - ptl > 8 && tpr - ptr > 8)
	{
		if (cmp(ptl + 7, ptr) <= 0)
		{
			loop = 8; do *ptd++ = *ptl++; while (--loop); continue;
		}
		if (cmp(ptl, ptr + 7) > 0)
		{
			loop = 8; do *ptd++ = *ptr++; while (--loop); continue;
		}
		if (cmp(tpl, tpr - 7) <= 0)
		{
			loop = 8; do *tpd-- = *tpr--; while (--loop); continue;
		}
		if (cmp(tpl - 7, tpr) > 0)
		{
			loop = 8; do *tpd-- = *tpl--; while (--loop); continue;
		}
		loop = 8; do
		{
			x = cmp(ptl, ptr) <= 0; y = !x; ptd[x] = *ptr; ptr += y; ptd[y] = *ptl; ptl += x; ptd++;
			x = cmp(tpl, tpr) <= 0; y = !x; tpd--; tpd[x] = *tpr; tpr -= x; tpd[y] = *tpl; tpl -= y;
		}
		while (--loop);
	}

	while (tpl - ptl > 1 && tpr - ptr > 1)
	{
		if (cmp(ptl + 1, ptr) <= 0)
		{
			*ptd++ = *ptl++; *ptd++ = *ptl++;
		}
		else if (cmp(ptl, ptr + 1) > 0)
		{
			*ptd++ = *ptr++; *ptd++ = *ptr++;
		}
		else 
		{
			x = cmp(ptl, ptr) <= 0; y = !x; ptd[x] = *ptr; ptr += 1; ptd[y] = *ptl; ptl += 1; ptd += 2;
			x = cmp(ptl, ptr) <= 0; y = !x; ptd[x] = *ptr; ptr += y; ptd[y] = *ptl; ptl += x; ptd++;
		}
	}

	while (ptl <= tpl && ptr <= tpr)
	{
		*ptd++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	}
	while (ptl <= tpl)
	{
		*ptd++ = *ptl++;
	}
	while (ptr <= tpr)
	{
		*ptd++ = *ptr++;
	}
}

// main memory: [A][B][C][D]
// swap memory: [A  B]       step 1
// swap memory: [A  B][C  D] step 2
// main memory: [A  B  C  D] step 3

void FUNC(quad_merge_block)(VAR *array, VAR *swap, size_t block, CMPFUNC *cmp)
{
	VAR *pts, *c, *c_max;
	size_t block_x_2 = block * 2;

	c_max = array + block;

	if (cmp(c_max - 1, c_max) <= 0)
	{
		c_max += block_x_2;

		if (cmp(c_max - 1, c_max) <= 0)
		{
			c_max -= block;

			if (cmp(c_max - 1, c_max) <= 0)
			{
				return;
			}
			pts = swap;

			c = array;

			do *pts++ = *c++; while (c < c_max); // step 1

			c_max = c + block_x_2;

			do *pts++ = *c++; while (c < c_max); // step 2

			return FUNC(galloping_merge)(array, swap, block_x_2, cmp); // step 3
		}
		pts = swap;

		c = array;
		c_max = array + block_x_2;

		do *pts++ = *c++; while (c < c_max); // step 1
	}
	else
	{
		FUNC(galloping_merge)(swap, array, block, cmp); // step 1
	}
	FUNC(galloping_merge)(swap + block_x_2, array + block_x_2, block, cmp); // step 2

	FUNC(galloping_merge)(array, swap, block_x_2, cmp); // step 3
}

size_t FUNC(quad_merge)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *pta, *pte;

	pte = array + nmemb;

	block *= 4;

	while (block <= nmemb && block <= swap_size)
	{
		pta = array;

		do
		{
			FUNC(quad_merge_block)(pta, swap, block / 4, cmp);

			pta += block;
		}
		while (pta + block <= pte);

		FUNC(tail_merge)(pta, swap, swap_size, pte - pta, block / 4, cmp);

		block *= 4;
	}

	FUNC(tail_merge)(array, swap, swap_size, nmemb, block / 4, cmp);

	return block / 2;
}

void FUNC(partial_forward_merge)(VAR *array, VAR *swap, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *r, *m, *e, *s; // right, middle, end, swap
	size_t x, y;

	r = array + block;
	e = array + nmemb - 1;

	memcpy(swap, array, block * sizeof(VAR));

	s = swap;
	m = swap + block - 1;

	while (s < m - 1 && r < e - 1)
	{
		if (cmp(s, r + 1) > 0)
		{
			*array++ = *r++; *array++ = *r++;
		}
		else if (cmp(s + 1, r) <= 0)
		{
			*array++ = *s++; *array++ = *s++;
		}
		else 
		{
			x = cmp(s, r) <= 0; y = !x; array[x] = *r; r += 1; array[y] = *s; s += 1; array += 2;
			x = cmp(s, r) <= 0; y = !x; array[x] = *r; r += y; array[y] = *s; s += x; array++;
		}
	}

	while (s <= m && r <= e)
	{
		*array++ = cmp(s, r) <= 0 ? *s++ : *r++;
	}

	while (s <= m)
	{
		*array++ = *s++;
	}
}

void FUNC(partial_backward_merge)(VAR *array, VAR *swap, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *m, *e, *s; // middle, end, swap
	size_t x, y;

	m = array + block - 1;
	e = array + nmemb - 1;

	if (cmp(m, m + 1) <= 0)
	{
		return;
	}

	memcpy(swap, array + block, (nmemb - block) * sizeof(VAR));

	s = swap + nmemb - block - 1;

	while (s > swap + 1 && m > array + 1)
	{
		if (cmp(m - 1, s) > 0)
		{
			*e-- = *m--;
			*e-- = *m--;
		}
		else if (cmp(m, s - 1) <= 0)
		{
			*e-- = *s--;
			*e-- = *s--;
		}
		else
		{
			x = cmp(m, s) <= 0; y = !x; e--; e[x] = *s; s -= 1; e[y] = *m; m -= 1; e--;
			x = cmp(m, s) <= 0; y = !x; e--; e[x] = *s; s -= x; e[y] = *m; m -= y;
		}
	}

	while (s >= swap && m >= array)
	{
		*e-- = cmp(m, s) > 0 ? *m-- : *s--;
	}

	while (s >= swap)
	{
		*e-- = *s--;
	}
}

void FUNC(tail_merge)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *pta, *pte;

	pte = array + nmemb;

	while (block < nmemb && block <= swap_size)
	{
		for (pta = array ; pta + block < pte ; pta += block * 2)
		{
			if (pta + block * 2 < pte)
			{
				FUNC(partial_backward_merge)(pta, swap, block * 2, block, cmp);

				continue;
			}
			FUNC(partial_backward_merge)(pta, swap, pte - pta, block, cmp);

			break;
		}
		block *= 2;
	}
}

// the next four functions provide in-place rotate merge support

void FUNC(trinity_rotation)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t left)
{
	size_t bridge, right = nmemb - left;

	if (left < right)
	{
		if (left <= swap_size)
		{
			memcpy(swap, array, left * sizeof(VAR));
			memmove(array, array + left, right * sizeof(VAR));
			memcpy(array + right, swap, left * sizeof(VAR));
		}
		else
		{
			VAR *pta, *ptb, *ptc, *ptd;

			pta = array;
			ptb = pta + left;

			bridge = right - left;

			if (bridge <= swap_size && bridge > 3)
			{
				ptc = pta + right;
				ptd = ptc + left;

				memcpy(swap, ptb, bridge * sizeof(VAR));

				while (left--)
				{
					*--ptc = *--ptd; *ptd = *--ptb;
				}
				memcpy(pta, swap, bridge * sizeof(VAR));
			}
			else
			{
				ptc = ptb;
				ptd = ptc + right;

				bridge = left / 2;

				while (bridge--)
				{
					*swap = *--ptb; *ptb = *pta; *pta++ = *ptc; *ptc++ = *--ptd; *ptd = *swap;
				}

				bridge = (ptd - ptc) / 2;

				while (bridge--)
				{
					*swap = *ptc; *ptc++ = *--ptd; *ptd = *pta; *pta++ = *swap;
				}

				bridge = (ptd - pta) / 2;

				while (bridge--)
				{
					*swap = *pta; *pta++ = *--ptd; *ptd = *swap;
				}
			}
		}
	}
	else if (right < left)
	{
		if (right <= swap_size)
		{
			memcpy(swap, array + left, right * sizeof(VAR));
			memmove(array + right, array, left * sizeof(VAR));
			memcpy(array, swap, right * sizeof(VAR));
		}
		else
		{
			VAR *pta, *ptb, *ptc, *ptd;

			pta = array;
			ptb = pta + left;

			bridge = left - right;

			if (bridge <= swap_size && bridge > 3)
			{
				ptc = pta + right;
				ptd = ptc + left;

				memcpy(swap, ptc, bridge * sizeof(VAR));

				while (right--)
				{
					*ptc++ = *pta; *pta++ = *ptb++;
				}
				memcpy(ptd - bridge, swap, bridge * sizeof(VAR));
			}
			else
			{
				ptc = ptb;
				ptd = ptc + right;

				bridge = right / 2;

				while (bridge--)
				{
					*swap = *--ptb; *ptb = *pta; *pta++ = *ptc; *ptc++ = *--ptd; *ptd = *swap;
				}

				bridge = (ptb - pta) / 2;

				while (bridge--)
				{
					*swap = *--ptb; *ptb = *pta; *pta++ = *--ptd; *ptd = *swap;
				}

				bridge = (ptd - pta) / 2;

				while (bridge--)
				{
					*swap = *pta; *pta++ = *--ptd; *ptd = *swap;
				}
			}
		}
	}
	else
	{
		VAR *pta, *ptb;

		pta = array;
		ptb = pta + left;

		while (left--)
		{
			*swap = *pta; *pta++ = *ptb; *ptb++ = *swap;
		}
	}
}

size_t FUNC(monobound_binary_first)(VAR *array, VAR *value, size_t top, CMPFUNC *cmp)
{
	VAR *end;
	size_t mid;

	end = array + top;

	while (top > 1)
	{
		mid = top / 2;

		if (cmp(value, end - mid) <= 0)
		{
			end -= mid;
		}
		top -= mid;
	}

	if (cmp(value, end - 1) <= 0)
	{
		end--;
	}
	return (end - array);
}

void FUNC(blit_merge_block)(VAR *array, VAR *swap, size_t swap_size, size_t lblock, size_t right, CMPFUNC *cmp)
{
	size_t left, rblock;

	if (cmp(array + lblock - 1, array + lblock) <= 0)
	{
		return;
	}

	rblock = lblock / 2;
	lblock -= rblock;

	left = FUNC(monobound_binary_first)(array + lblock + rblock, array + lblock, right, cmp);

	right -= left;

	if (left)
	{
		FUNC(trinity_rotation)(array + lblock, swap, swap_size, rblock + left, rblock);

		if (left <= swap_size)
		{
			FUNC(partial_backward_merge)(array, swap, lblock + left, lblock, cmp);
		}
		else if (lblock <= swap_size)
		{
			FUNC(partial_forward_merge)(array, swap, lblock + left, lblock, cmp);
		}
		else
		{
			FUNC(blit_merge_block)(array, swap, swap_size, lblock, left, cmp);
		}
	}

	if (right)
	{
		if (right <= swap_size)
		{
			FUNC(partial_backward_merge)(array + lblock + left, swap, rblock + right, rblock, cmp);
		}
		else if (rblock <= swap_size)
		{
			FUNC(partial_forward_merge)(array + lblock + left, swap, rblock + right, rblock, cmp);
		}
		else
		{
			FUNC(blit_merge_block)(array + lblock + left, swap, swap_size, rblock, right, cmp);
		}
	}
}

void FUNC(blit_merge)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *pta, *pte;

	pte = array + nmemb;

	while (block < nmemb)
	{
		for (pta = array ; pta + block < pte ; pta += block * 2)
		{
			if (pta + block * 2 < pte)
			{
				FUNC(blit_merge_block)(pta, swap, swap_size, block, block, cmp);

				continue;
			}
			FUNC(blit_merge_block)(pta, swap, swap_size, block, pte - pta - block, cmp);

			break;
		}
		block *= 2;
	}
}

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

void FUNC(quadsort)(void *array, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb < 32)
	{
		FUNC(tail_swap)(array, nmemb, cmp);
	}
	else if (FUNC(quad_swap)(array, nmemb, cmp) == 0)
	{
		VAR *swap;
		size_t swap_size = 32;

		while (swap_size * 4 <= nmemb)
		{
			swap_size *= 4;
		}
		swap = malloc(swap_size * sizeof(VAR));

		if (swap == NULL)
		{
			swap = malloc((swap_size = 512) * sizeof(VAR));

			if (swap == NULL)
			{
				VAR stack[32];

				FUNC(tail_merge)(array, stack, 32, nmemb, 32, cmp);

				FUNC(blit_merge)(array, stack, 32, nmemb, 64, cmp);

				return;
			}
		}
		FUNC(quad_merge)(array, swap, swap_size, nmemb, 32, cmp);

		FUNC(blit_merge)(array, swap, swap_size, nmemb, swap_size * 2, cmp);

		free(swap);
	}
}

void FUNC(quadsort_swap)(void *array, void *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb < 32)
	{
		FUNC(tail_swap)(array, nmemb, cmp);
	}
	else if (FUNC(quad_swap)(array, nmemb, cmp) == 0)
	{
		size_t block;

		block = FUNC(quad_merge)(array, swap, swap_size, nmemb, 32, cmp);

		FUNC(blit_merge)(array, swap, swap_size, nmemb, block, cmp);
	}
}
