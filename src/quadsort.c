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

void FUNC(tail_swap)(VAR *array, unsigned char nmemb, CMPFUNC *cmp)
{
	VAR *pta, *end, *ptt, tmp;
	unsigned char mid, top, offset;

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
			swap_five(array, pta, ptt, end, tmp, cmp);
			return;

		case 6:
			swap_six(array, pta, ptt, end, tmp, cmp);
			return;

		case 7:
			swap_seven(array, pta, ptt, end, tmp, cmp);
			return;
		case 8:
			swap_eight(array, pta, ptt, end, tmp, cmp);
			return;
	}
	swap_eight(array, pta, ptt, end, tmp, cmp);

	end = array + 8;
	offset = 8;

	while (offset < nmemb)
	{
		top = offset++;
		pta = end++;
		ptt = pta--;

		if (cmp(pta, ptt) <= 0)
		{
			continue;
		}

		tmp = *ptt;

		while (top > 1)
		{
			mid = top / 2;

			if (cmp(pta - mid, &tmp) > 0)
			{
				pta -= mid;
			}
			top -= mid;
		}

		memmove(pta + 1, pta, (ptt - pta) * sizeof(VAR));

		*pta = tmp;
	}
}

void FUNC(tail_merge)(VAR *array, VAR *swap, unsigned int nmemb, unsigned int block, CMPFUNC *cmp);
void FUNC(quad_merge_block)(VAR *array, VAR *swap, unsigned int nmemb, unsigned int block, CMPFUNC *cmp);

unsigned int FUNC(quad_swap)(VAR *array, unsigned int nmemb, CMPFUNC *cmp)
{
	VAR swap[32];
	unsigned int count, reverse;
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
		pts = pta;
		pta += 4;

		if (count--)
		{
			tail_swap_eight(pts, ptt, pte, pta, tmp, cmp);
			tail_swap_eight(pts, ptt, pte, pta, tmp, cmp);
			tail_swap_eight(pts, ptt, pte, pta, tmp, cmp);
			tail_swap_eight(pts, ptt, pte, pta, tmp, cmp);
	
			continue;
		}
		break;

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

			pts = pta;

			if (count & 1)
			{
				count--;
				pta += 4;
			}
			else
			{
				pts -= 4;
			}

			tail_swap_eight(pts, ptt, pte, pta, tmp, cmp);
			tail_swap_eight(pts, ptt, pte, pta, tmp, cmp);
			tail_swap_eight(pts, ptt, pte, pta, tmp, cmp);
			tail_swap_eight(pts, ptt, pte, pta, tmp, cmp);

			continue;
		}

		if (pts == array)
		{
			switch (nmemb & 3)
			{
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
		FUNC(quad_merge_block)(pta, swap, 32, 8, cmp);

		pta += 32;
	}

	if (nmemb % 32 > 8)
	{
		FUNC(tail_merge)(pta, swap, nmemb % 32, 8, cmp);
	}

	return 0;
}

void FUNC(forward_merge)(VAR *dest, VAR *from, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *l, *r, *m, *e; // left, right, middle, end

	l = from;
	r = from + block;
	m = r;
	e = l + nmemb;

	while (1)
	{
		if (cmp(l, r) <= 0)
		{
			*dest++ = *l++;

			if (l == m)
			{
				do *dest++ = *r++; while (r < e);

				return;
			}
		}
		else
		{
			*dest++ = *r++;

			if (r == e)
			{
				do *dest++ = *l++; while (l < m);

				return;
			}
		}
	}
}

// main memory: [A][B][C][D]
// swap memory: [A  B]       step 1
// swap memory: [A  B][C  D] step 2
// main memory: [A  B  C  D] step 3

void FUNC(quad_merge_block)(VAR *array, VAR *swap, unsigned int nmemb, unsigned int block, CMPFUNC *cmp)
{
	register VAR *pts, *c, *c_max;
	unsigned int block_x_2 = block * 2;

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

			return FUNC(forward_merge)(array, swap, block_x_2 * 2, block_x_2, cmp); // step 3
		}
		pts = swap;

		c = array;
		c_max = array + block * 2;

		do *pts++ = *c++; while (c < c_max); // step 1
	}
	else
	{
		FUNC(forward_merge)(swap, array, block_x_2, block, cmp); // step 1
	}
	FUNC(forward_merge)(swap + block_x_2, array + block_x_2, block_x_2, block, cmp); // step 2

	FUNC(forward_merge)(array, swap, block_x_2 * 2, block_x_2, cmp); // step 3
}

void FUNC(quad_merge)(VAR *array, VAR *swap, unsigned int nmemb, unsigned int block, CMPFUNC *cmp)
{
	register VAR *pta, *pte;

	pte = array + nmemb;

	block *= 4;

	while (block * 2 <= nmemb)
	{
		pta = array;

		do
		{
			FUNC(quad_merge_block)(pta, swap, block, block / 4, cmp);

			pta += block;
		}
		while (pta + block <= pte);

		FUNC(tail_merge)(pta, swap, pte - pta, block / 4, cmp);

		block *= 4;
	}
	FUNC(tail_merge)(array, swap, nmemb, block / 4, cmp);
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
		while (1)
		{
			if (cmp(m, s) > 0)
			{
				*e-- = *m--;
			}
			else
			{
				*e-- = *s--;

				if (s < swap)
				{
					return;
				}
			}
		}
	}
	else
	{
		while (1)
		{
			if (cmp(m, s) > 0)
			{
				*e-- = *m--;

				if (m < array)
				{
					do *e-- = *s--; while (s >= swap);

					return;
				}
			}
			else
			{
				*e-- = *s--;
			}
		}
	}
}

void FUNC(tail_merge)(VAR *array, VAR *swap, unsigned int nmemb, unsigned int block, CMPFUNC *cmp)
{
	register VAR *pta, *pte;

	pte = array + nmemb;

	while (block < nmemb)
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
	if (nmemb <= 8)
	{
		FUNC(tail_swap)(array, nmemb, cmp);
	}
	else if (nmemb < 256)
	{
		if (FUNC(quad_swap)(array, nmemb, cmp) == 0)
		{
			VAR swap[128];

			FUNC(tail_merge)(array, swap, nmemb, 32, cmp);
		}
	}
	else if (FUNC(quad_swap)(array, nmemb, cmp) == 0)
	{
		VAR *swap = malloc(nmemb * sizeof(VAR) / 2);

		if (swap == NULL)
		{
			fprintf(stderr, "quadsort(%p,%zu,%p): malloc() failed: %s\n", array, nmemb, cmp, strerror(errno));

			return;
		}

		FUNC(quad_merge)(array, swap, nmemb, 32, cmp);

		free(swap);
	}
}

void FUNC(quadsort_swap)(VAR *array, VAR *swap, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb <= 8)
	{
		FUNC(tail_swap)(array, nmemb, cmp);
	}
	else if (nmemb < 256)
	{
		if (FUNC(quad_swap)(array, nmemb, cmp) == 0)
		{
			FUNC(tail_merge)(array, swap, nmemb, 32, cmp);
		}
	}
	else if (FUNC(quad_swap)(array, nmemb, cmp) == 0)
	{
		FUNC(quad_merge)(array, swap, nmemb, 32, cmp);
	}
}
