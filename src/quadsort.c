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
	quadsort 1.1.4.4
*/

void FUNC(unguarded_insert)(VAR *array, size_t offset, size_t nmemb, CMPFUNC *cmp)
{
	VAR key, *pta, *end;
	size_t i, top;

	for (i = offset ; i < nmemb ; i++)
	{
		pta = end = array + i;

		if (cmp(--pta, end) <= 0)
		{
			continue;
		}

		key = *end;

		if (cmp(array, &key) > 0)
		{
			top = i;

			do
			{
				*end-- = *pta--;
			}
			while (--top);

			*end = key;
		}
		else
		{
			do
			{
				*end-- = *pta--;
			}
			while (cmp(pta, &key) > 0);

			*end = key;
		}
	}
}

void FUNC(tail_swap)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	VAR *pta, *end, *ptt, tmp;

	switch (nmemb)
	{
		case 0:
		case 1:
			return;

		case 2:
			swap_two(array, tmp);
			return;

		case 3:
			swap_three(array, tmp);
			return;

		case 4:
			swap_four(array, tmp);
			return;

		case 5:
			swap_four(array, tmp);
			swap_five(array, pta, ptt, end, tmp, cmp);
			return;

		case 6:
			swap_four(array, tmp);
			swap_six(array, pta, ptt, end, tmp, cmp);
			return;

		case 7:
			swap_four(array, tmp);
			swap_seven(array, pta, ptt, end, tmp, cmp);
			return;
		case 8:
			swap_four(array, tmp);
			swap_eight(array, pta, ptt, end, tmp, cmp);
			return;
	}
	swap_four(array, tmp);
	swap_eight(array, pta, ptt, end, tmp, cmp);

	FUNC(unguarded_insert)(array, 8, nmemb, cmp);
}

void FUNC(tail_merge)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t block, CMPFUNC *cmp);
void FUNC(parity_merge_thirtytwo)(VAR *array, VAR *swap, CMPFUNC *cmp);

size_t FUNC(quad_swap)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	VAR swap[32];
	size_t count, reverse;
	VAR *pta, *pts, *ptt, *pte, tmp;

	pta = array;

	count = nmemb / 4;

	count &= ~1;

	while (count--)
	{
		if (cmp(&pta[0], &pta[1]) > 0)
		{
			if (cmp(&pta[2], &pta[3]) > 0)
			{
				if (cmp(&pta[1], &pta[2]) > 0)
				{
					pts = pta;
					pta += 4;
					goto swapper;
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
			if (cmp(&pta[0], &pta[2]) <= 0)
			{
				if (cmp(&pta[1], &pta[3]) <= 0)
				{
					tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
				else
				{
					tmp = pta[1]; pta[1] = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
				}
			}
			else if (cmp(&pta[0], &pta[3]) > 0)
			{
				tmp = pta[1]; pta[1] = pta[3]; pta[3] = tmp; tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
			}
			else if (cmp(&pta[1], &pta[3]) <= 0)
			{
				tmp = pta[1]; pta[1] = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
			}
			else
			{
				tmp = pta[1]; pta[1] = pta[0]; pta[0] = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
			}
		}
		count--;

		pts = pta;

		swap_eight(pts, ptt, pte, pta, tmp, cmp);

		continue;

		swapper:

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
							pta += 4;

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
				if (cmp(&pta[0], &pta[2]) <= 0)
				{
					if (cmp(&pta[1], &pta[3]) <= 0)
					{
						tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
					}
					else
					{
						tmp = pta[1]; pta[1] = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
					}
				}
				else if (cmp(&pta[0], &pta[3]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp; tmp = pta[1]; pta[1] = pta[3]; pta[3] = tmp;
				}
				else if (cmp(&pta[1], &pta[3]) <= 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = pta[1]; pta[1] = tmp;
				}
				else
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = pta[3]; pta[3] = pta[1]; pta[1] = tmp;
				}
			}
			ptt = pta - 1;

			reverse = (ptt - pts) / 2;

			do
			{
				tmp = *pts; *pts++ = *ptt; *ptt-- = tmp;
			}
			while (reverse--);

			pts = count & 1 ? pta : pta - 4;

			count &= ~1;

			swap_eight(pts, ptt, pte, pta, tmp, cmp);

			continue;
		}

		if (pts == array)
		{
			switch (nmemb & 7)
			{
				case 7:
					if (cmp(&pta[5], &pta[6]) <= 0)
					{
						break;
					}
				case 6:
					if (cmp(&pta[4], &pta[5]) <= 0)
					{
						break;
					}
				case 5:
					if (cmp(&pta[3], &pta[4]) <= 0)
					{
						break;
					}
				case 4:
					if (cmp(&pta[2], &pta[3]) <= 0)
					{
						break;
					}
				case 3:
					if (cmp(&pta[1], &pta[2]) <= 0)
					{
						break;
					}
				case 2:
					if (cmp(&pta[0], &pta[1]) <= 0)
					{
						break;
					}
				case 1:
					if (cmp(&pta[-1], &pta[0]) <= 0)
					{
						break;
					}
				case 0:
					ptt = pts + nmemb - 1;

					reverse = (ptt - pts) / 2;

					do
					{
						tmp = *pts; *pts++ = *ptt; *ptt-- = tmp;
					}
					while (reverse--);

					return 1;
			}
		}
		ptt = pta - 1;

		reverse = (ptt - pts) / 2;

		do
		{
			tmp = *pts; *pts++ = *ptt; *ptt-- = tmp;
		}
		while (reverse--);

		break;
	}

	FUNC(tail_swap)(pta, nmemb & 7, cmp);

	pta = array;

	count = nmemb / 32;

	while (count--)
	{
		FUNC(parity_merge_thirtytwo)(pta, swap, cmp);

		pta += 32;
	}

	if ((nmemb & 31) > 8)
	{
		FUNC(tail_merge)(pta, swap, 32, nmemb & 31, 8, cmp);
	}

	return 0;
}

void FUNC(parity_merge_eight)(VAR *dest, VAR *from, CMPFUNC *cmp)
{
	VAR *ptl, *ptr;
	unsigned char x, y;

	ptl = from;
	ptr = from + 8;

	x = cmp(ptl, ptr) <= 0; y = !x; dest[x] = *ptr; dest[y] = *ptl; ptr += y; ptl += x; dest++;
	x = cmp(ptl, ptr) <= 0; y = !x; dest[x] = *ptr; dest[y] = *ptl; ptr += y; ptl += x; dest++;
	x = cmp(ptl, ptr) <= 0; y = !x; dest[x] = *ptr; dest[y] = *ptl; ptr += y; ptl += x; dest++;
	x = cmp(ptl, ptr) <= 0; y = !x; dest[x] = *ptr; dest[y] = *ptl; ptr += y; ptl += x; dest++;
	x = cmp(ptl, ptr) <= 0; y = !x; dest[x] = *ptr; dest[y] = *ptl; ptr += y; ptl += x; dest++;
	x = cmp(ptl, ptr) <= 0; y = !x; dest[x] = *ptr; dest[y] = *ptl; ptr += y; ptl += x; dest++;
	x = cmp(ptl, ptr) <= 0; y = !x; dest[x] = *ptr; dest[y] = *ptl; ptr += y; ptl += x; dest++;

	*dest = cmp(ptl, ptr) <= 0 ? *ptl : *ptr;

	dest += 8;

	ptl = from + 7;
	ptr = from + 15;

	x = cmp(ptl, ptr) <= 0; y = !x; dest--; dest[x] = *ptr; dest[y] = *ptl; ptl -= y; ptr -= x;
	x = cmp(ptl, ptr) <= 0; y = !x; dest--; dest[x] = *ptr; dest[y] = *ptl; ptl -= y; ptr -= x;
	x = cmp(ptl, ptr) <= 0; y = !x; dest--; dest[x] = *ptr; dest[y] = *ptl; ptl -= y; ptr -= x;
	x = cmp(ptl, ptr) <= 0; y = !x; dest--; dest[x] = *ptr; dest[y] = *ptl; ptl -= y; ptr -= x;
	x = cmp(ptl, ptr) <= 0; y = !x; dest--; dest[x] = *ptr; dest[y] = *ptl; ptl -= y; ptr -= x;
	x = cmp(ptl, ptr) <= 0; y = !x; dest--; dest[x] = *ptr; dest[y] = *ptl; ptl -= y; ptr -= x;
	x = cmp(ptl, ptr) <= 0; y = !x; dest--; dest[x] = *ptr; dest[y] = *ptl; ptl -= y; ptr -= x;

	*dest = cmp(ptl, ptr) > 0 ? *ptl : *ptr;
}

void FUNC(parity_merge_sixteen)(VAR *dest, VAR *from, CMPFUNC *cmp)
{
	VAR *ptl, *ptr, *pte;
	unsigned char x, y;

	ptl = from;
	ptr = from + 16;
	pte = dest + 15;

	do
	{
		x = cmp(ptl, ptr) <= 0; y = !x; dest[x] = *ptr; ptr += y; dest[y] = *ptl; ptl += x; dest++;
	}
	while (dest < pte);

	*dest = cmp(ptl, ptr) <= 0 ? *ptl : *ptr;

	dest += 16;

	ptl = from + 15;
	ptr = from + 31;
	pte = dest - 15;

	do
	{
		x = cmp(ptl, ptr) <= 0; y = !x; dest--; dest[x] = *ptr; ptr -= x; dest[y] = *ptl; ptl -= y;
	}
	while (dest > pte);

	*dest = cmp(ptl, ptr) > 0 ? *ptl : *ptr;
}

void FUNC(parity_merge_thirtytwo)(VAR *array, VAR *swap, CMPFUNC *cmp)
{
	if (cmp(array + 7, array + 8) <= 0 && cmp(array + 15, array + 16) <= 0 && cmp(array + 23, array + 24) <= 0)
	{
		return;
	}
	FUNC(parity_merge_eight)(swap, array, cmp);
	FUNC(parity_merge_eight)(swap + 16, array + 16, cmp);

	FUNC(parity_merge_sixteen)(array, swap, cmp);
}

void FUNC(partial_forward_merge)(VAR *array, VAR *swap, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *r, *m, *e, *s; // right, middle, end, swap

	r = array + block;
	e = array + nmemb - 1;

	memcpy(swap, array, block * sizeof(VAR));

	s = swap;
	m = swap + block - 1;

	if (cmp(m, e) <= 0)
	{
		do
		{
			while (cmp(s, r) > 0)
			{
				*array++ = *r++;
			}
			*array++ = *s++;
		}
		while (s <= m);
	}
	else
	{
		do
		{
			if (cmp(s, r) > 0)
			{
				*array++ = *r++;
				continue;
			}
			*array++ = *s++;

			if (cmp(s, r) > 0)
			{
				*array++ = *r++;
				continue;
			}
			*array++ = *s++;

			if (cmp(s, r) > 0)
			{
				*array++ = *r++;
				continue;
			}
			*array++ = *s++;
		}
		while (r <= e);

		do *array++ = *s++; while (s <= m);
	}
}

void FUNC(partial_backward_merge)(VAR *array, VAR *swap, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *r, *m, *e, *s; // right, middle, end, swap

	m = array + block;
	e = array + nmemb - 1;
	r = m--;

	if (cmp(m, r) <= 0)
	{
		return;
	}

	while (cmp(m, e) <= 0)
	{
		e--;
	}

	s = swap;

	do *s++ = *r++; while (r <= e);

	s--;

	*e-- = *m--;

	if (cmp(array, swap) <= 0)
	{
		do
		{
			while (cmp(m, s) > 0)
			{
				*e-- = *m--;
			}
			*e-- = *s--;
		}
		while (s >= swap);
	}
	else
	{
		do
		{
			if (cmp(m, s) > 0)
			{
				*e-- = *m--;
				continue;
			}
			*e-- = *s--;
			if (cmp(m, s) > 0)
			{
				*e-- = *m--;
				continue;
			}
			*e-- = *s--;
			if (cmp(m, s) > 0)
			{
				*e-- = *m--;
				continue;
			}
			*e-- = *s--;
		}
		while (m >= array);

		do *e-- = *s--; while (s >= swap);
	}
}

void FUNC(forward_merge)(VAR *dest, VAR *from, size_t block, CMPFUNC *cmp)
{
	VAR *l, *r, *m, *e; // left, right, middle, end

	l = from;
	r = from + block;
	m = r;
	e = r + block;

	if (cmp(m - 1, e - 1) <= 0)
	{
		do
		{
			if (cmp(l, r) <= 0)
			{
				*dest++ = *l++;
				continue;
			}
			*dest++ = *r++;
			if (cmp(l, r) <= 0)
			{
				*dest++ = *l++;
				continue;
			}
			*dest++ = *r++;
			if (cmp(l, r) <= 0)
			{
				*dest++ = *l++;
				continue;
			}
			*dest++ = *r++;
		}
		while (l < m);

		do *dest++ = *r++; while (r < e);
	}
	else
	{
		do
		{
			if (cmp(l, r) > 0)
			{
				*dest++ = *r++;
				continue;
			}
			*dest++ = *l++;
			if (cmp(l, r) > 0)
			{
				*dest++ = *r++;
				continue;
			}
			*dest++ = *l++;
			if (cmp(l, r) > 0)
			{
				*dest++ = *r++;
				continue;
			}
			*dest++ = *l++;
		}
		while (r < e);

		do *dest++ = *l++; while (l < m);
	}
}

// main memory: [A][B][C][D]
// swap memory: [A  B]       step 1
// swap memory: [A  B][C  D] step 2
// main memory: [A  B  C  D] step 3

void FUNC(quad_merge_block)(VAR *array, VAR *swap, size_t block, CMPFUNC *cmp)
{
	register VAR *pts, *c, *c_max;
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

			return FUNC(forward_merge)(array, swap, block_x_2, cmp); // step 3
		}
		pts = swap;

		c = array;
		c_max = array + block_x_2;

		do *pts++ = *c++; while (c < c_max); // step 1
	}
	else
	{
		FUNC(forward_merge)(swap, array, block, cmp); // step 1
	}
	FUNC(forward_merge)(swap + block_x_2, array + block_x_2, block, cmp); // step 2

	FUNC(forward_merge)(array, swap, block_x_2, cmp); // step 3
}

void FUNC(quad_merge)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	register VAR *pta, *pte;

	pte = array + nmemb;

	block *= 4;

	while (block < nmemb && block <= swap_size)
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
}

void FUNC(tail_merge)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	register VAR *pta, *pte;

	pte = array + nmemb;

	while (block < nmemb && block <= swap_size)
	{
		pta = array;

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

// rotate merge support routines

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

			if (bridge <= swap_size && bridge > 2)
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

			if (bridge <= swap_size && bridge > 2)
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

void FUNC(blit_merge_block)(VAR *array, VAR *swap, size_t swap_size, size_t block, size_t right, CMPFUNC *cmp)
{
	size_t left;

	if (cmp(array + block - 1, array + block) <= 0)
	{
		return;
	}

	left = FUNC(monobound_binary_first)(array + block, array + block / 2, right, cmp);

	right -= left;

	block /= 2;

	if (left)
	{
		FUNC(trinity_rotation)(array + block, swap, swap_size, block + left, block);

		if (left <= swap_size)
		{
			FUNC(partial_backward_merge)(array, swap, block + left, block, cmp);
		}
		else if (block <= swap_size)
		{
			FUNC(partial_forward_merge)(array, swap, block + left, block, cmp);
		}
		else
		{
			FUNC(blit_merge_block)(array, swap, swap_size, block, left, cmp);
		}
	}

	if (right)
	{
		if (right <= swap_size)
		{
			FUNC(partial_backward_merge)(array + block + left, swap, block + right, block, cmp);
		}
		else if (block <= swap_size)
		{
			FUNC(partial_forward_merge)(array + block + left, swap, block + right, block, cmp);
		}
		else
		{
			FUNC(blit_merge_block)(array + block + left, swap, swap_size, block, right, cmp);
		}
	}
}

void FUNC(blit_merge)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *pta, *pte;

	pte = array + nmemb;

	while (block < nmemb)
	{
		pta = array;

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

		while (swap_size * 8 < nmemb)
		{
			swap_size *= 2;
		}
		swap = malloc(swap_size * sizeof(VAR));

		if (swap == NULL)
		{
			swap = malloc((swap_size = 1024) * sizeof(VAR));

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

void FUNC(quadsort_swap)(void *array, void *swap, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb < 32)
	{
		FUNC(tail_swap)(array, nmemb, cmp);
	}
	else if (FUNC(quad_swap)(array, nmemb, cmp) == 0)
	{
		FUNC(quad_merge)(array, swap, nmemb, nmemb, 32, cmp);
	}
}
