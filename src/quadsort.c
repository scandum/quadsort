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
	quadsort 1.1.3.4
*/

void FUNC(tail_insert)(VAR *array, VAR *pta, CMPFUNC *cmp)
{
	VAR *pte, key;

	pte = pta--;

	if (cmp(pta, pte) > 0)
	{
		key = *pte;

		if (cmp(pta - 3, &key) > 0)
		{
			*pte-- = *pta--;
			*pte-- = *pta--;
			*pte-- = *pta--;
			*pte-- = *pta--;
		}
		
		if (pta >= array + 1 && cmp(pta - 1, &key) > 0)
		{
			*pte-- = *pta--;
			*pte-- = *pta--;
		}

		if (pta >= array && cmp(pta, &key) > 0)
		{
			*pte-- = *pta;
		}
		*pte = key;
	}
}

void FUNC(tail_swap)(VAR *array, unsigned char nmemb, CMPFUNC *cmp)
{
	VAR pts[8];
	register VAR *pta = array;
	unsigned char i, mid, cnt;

	switch (nmemb)
	{
		case 0:
		case 1:
			return;

		case 2:
			swap_two(pta, *pts);
			return;

		case 3:
			swap_three(pta, *pts);
			return;

		case 4:
			swap_four(pta, *pts);
			return;
	}
	swap_four(pta, *pts);

	if (nmemb <= 8)
	{
		for (i = 4 ; i < nmemb ; i++)
		{
			FUNC(tail_insert)(pta, &pta[i], cmp);
		}
		return;
	}

	pta += 4;

	swap_four(pta, *pts);

	pta += 4;

	switch (nmemb)
	{
		case 9:
			break;

		case 10:
			swap_two(pta, *pts);
			break;

		case 11:
			swap_three(pta, *pts);
			break;

		default:
			swap_four(pta, *pts);

			for (i = 4 ; i + 8 < nmemb ; i++)
			{
				FUNC(tail_insert)(pta, &pta[i], cmp);
			}
			break;
	}

	pta -= 8;

	if (cmp(&pta[3], &pta[4]) <= 0)
	{
		if (cmp(&pta[7], &pta[8]) <= 0)
		{
			return;
		}
		for (i = 0 ; i < 8 ; i++)
		{
			pts[i] = pta[i];
		}
	}
	else if (cmp(&pta[0], &pta[7]) > 0)
	{
		pts[0] = pta[4];
		pts[1] = pta[5];
		pts[2] = pta[6];
		pts[3] = pta[7];

		pts[4] = pta[0];
		pts[5] = pta[1];
		pts[6] = pta[2];
		pts[7] = pta[3];
	}
	else
	{
		i = 0;
		mid = 4;

		pts[0] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		pts[1] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		pts[2] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		pts[3] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		pts[4] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];

		cnt = 5;

		while (i < 4 && mid < 8)
		{
			pts[cnt++] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		}
		while (i < 4)
		{
			pts[cnt++] = pta[i++];
		}
		while (mid < 8)
		{
			pts[cnt++] = pta[mid++];
		}
	}

	i = 0;
	mid = 8;

	pta[0] = cmp(&pts[i], &pta[mid]) > 0 ? pta[mid++] : pts[i++];

	cnt = 1;

	while (i < 8 && mid < nmemb)
	{
		pta[cnt++] = cmp(&pts[i], &pta[mid]) > 0 ? pta[mid++] : pts[i++];
	}
	while (i < 8)
	{
		pta[cnt++] = pts[i++];
	}
}

void FUNC(tail_merge)(VAR *array, VAR *swap, unsigned int nmemb, unsigned int block, CMPFUNC *cmp);
void FUNC(quad_merge_sixteen)(VAR *pte, VAR *swap, CMPFUNC *cmp);

unsigned int FUNC(quad_swap)(VAR *array, unsigned int nmemb, CMPFUNC *cmp)
{
	VAR swap[16];
	unsigned int offset;
	register VAR *pta, *pts, *ptt, *pte, tmp;

	pta = array;
	pte = pta + nmemb - 4;

	offset = 0;

	while (pta <= pte)
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
			tmp = pta[1];

			if (cmp(&pta[0], &pta[2]) <= 0)
			{
				if (cmp(&pta[1], &pta[3]) <= 0)
				{
					pta[1] = pta[2]; pta[2] = tmp;
				}
				else
				{
					pta[1] = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
				}
			}
			else if (cmp(&pta[0], &pta[3]) > 0)
			{
				pta[1] = pta[3]; pta[3] = tmp; tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
			}
			else if (cmp(&pta[1], &pta[3]) <= 0)
			{
				pta[1] = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
			}
			else
			{
				pta[1] = pta[0]; pta[0] = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
			}
		}
		pta += 4;

		continue;

		swapper:

		if (pta <= pte)
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
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
					tmp = pta[1]; pta[1] = pta[3]; pta[3] = tmp;

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

			if (pts == array)
			{
				offset = (pta - pts) & ~15;
			}
			ptt = pta - 1;
			pta += 4;
		}
		else
		{
			if (pts == array)
			{
				switch (nmemb & 3)
				{
					case 3:
						if (cmp(&pta[1], &pta[2]) <= 0)
						{
							offset = (pta - pts) & ~15;
							break;
						}
					case 2:
						if (cmp(&pta[0], &pta[1]) <= 0)
						{
							offset = (pta - pts) & ~15;
							break;
						}
					case 1:
						if (cmp(&pta[-1], &pta[0]) <= 0)
						{
							offset = (pta - pts) & ~15;
							break;
						}
					case 0:
						goto swapped;
				}
			}
			ptt = pta - 1;
		}

		while (pts < ptt)
		{
			tmp = *pts;
			*pts++ = *ptt;
			*ptt-- = tmp;
		}
		continue;

		swapped:

		ptt = pts + nmemb - 1;

		while (pts < ptt)
		{
			tmp = *pts;
			*pts++ = *ptt;
			*ptt-- = tmp;
		}
		return 1;
	}

	FUNC(tail_swap)(pta, nmemb % 4, cmp);

	pte = array + offset;

	for (tmp = (nmemb - offset) / 16 ; tmp > 0 ; tmp--)
	{
		FUNC(quad_merge_sixteen)(pte, swap, cmp);

		pte += 16;
	}

	if (nmemb % 16 > 4)
	{
		FUNC(tail_merge)(pte, swap, nmemb % 16, 4, cmp);
	}

	return 0;
}

void FUNC(quad_merge_sixteen)(VAR *pte, VAR *swap, CMPFUNC *cmp)
{
	VAR *pta, *pts, *ptt;

	VAR *swap_p_8 = swap + 8, *swap_p_16 = swap + 16;

	pta = pte;

	if (cmp(&pta[3], &pta[4]) <= 0)
	{
		if (cmp(&pta[11], &pta[12]) <= 0)
		{
			if (cmp(&pta[7], &pta[8]) <= 0)
			{
				return;
			}
			pts = swap;

			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

			goto step3;
		}
		pts = swap;

		*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
		*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

		goto step2;
	}

	// step1:

	pts = swap;

	if (cmp(&pta[3], &pta[7]) <= 0)
	{
		ptt = pta + 4;

		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;

		while (pta < pte + 4)
		{
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		}

		while (pts < swap_p_8)
		{
			*pts++ = *ptt++;
		}
		pta = ptt;
	}
	else if (cmp(&pta[0], &pta[7]) > 0)
	{
		ptt = pta + 4;
		*pts++ = *ptt++; *pts++ = *ptt++; *pts++ = *ptt++; *pts++ = *ptt++;
		*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
		pta = ptt;
	}
	else
	{
		ptt = pta + 4;

		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;

		while (ptt < pte + 8)
		{
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		}

		while (pts < swap_p_8)
		{
			*pts++ = *pta++;
		}
		pta = ptt;
	}

	if (cmp(&pta[3], &pta[4]) <= 0)
	{
		*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
		*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

		goto step3;
	}

	step2:

	if (cmp(&pta[3], &pta[7]) <= 0)
	{
		ptt = pta + 4;

		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;

		while (pta < pte + 12)
		{
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		}
		while (pts < swap_p_16)
		{
			*pts++ = *ptt++;
		}
	}
	else if (cmp(&pta[0], &pta[7]) > 0)
	{
		ptt = pta + 4;

		*pts++ = *ptt++; *pts++ = *ptt++; *pts++ = *ptt++; *pts++ = *ptt++;
		*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
	}
	else
	{
		ptt = pta + 4;

		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;

		while (ptt < pte + 16)
		{
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
		}
		while (pts < swap_p_16)
		{
			*pts++ = *pta++;
		}
	}

	step3:

	pta = pte;
	pts = swap;

	if (cmp(&pts[7], &pts[15]) <= 0)
	{
		ptt = swap_p_8;

		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

//		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

		while (pts < swap_p_8)
		{
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		}

		while (ptt < swap_p_16)
		{
			*pta++ = *ptt++;
		}
	}
	else if (cmp(&pts[0], &pts[15]) > 0)
	{
		ptt = swap_p_8;

		*pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++;
		*pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++;

		*pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++;
		*pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++;
	}
	else
	{
		ptt = swap_p_8;

		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

		*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

		while (ptt < swap_p_16)
		{
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
		}
		while (pts < swap_p_8)
		{
			*pta++ = *pts++;
		}
	}
}

void FUNC(quad_merge_block)(VAR *array, VAR *swap, unsigned int nmemb, unsigned int block, CMPFUNC *cmp)
{
	register VAR *pts, *c, *c_max, *d, *d_max;

	c_max = array + block;

	if (cmp(c_max - 1, c_max) <= 0)
	{
		c_max += block * 2;

		if (cmp(c_max - 1, c_max) <= 0)
		{
			c_max -= block;

			if (cmp(c_max - 1, c_max) <= 0)
			{
				return;
			}
			pts = swap;

			c = array;

			while (c < c_max - 8)
			{
				*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
				*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
			}
			while (c < c_max)
			{
				*pts++ = *c++;
			}
			c = c_max;
			c_max = c + block * 2;

			while (c < c_max - 8)
			{
				*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
				*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
			}
			while (c < c_max)
			{
				*pts++ = *c++;
			}
			goto step3;
		}
		pts = swap;

		c = array;
		c_max = array + block * 2;

		while (c < c_max)
		{
			*pts++ = *c++;
		}
		goto step2;
	}

	// step1:

	pts = swap;

	c = array;

	d = c_max;
	d_max = d + block;

	if (cmp(c_max - 1, d_max - 1) <= 0)
	{
		while (c < c_max)
		{
			while (cmp(c, d) > 0)
			{
				*pts++ = *d++;
			}
			*pts++ = *c++;
		}
		while (d < d_max)
		{
			*pts++ = *d++;
		}
	}
	else
	{
		while (d < d_max)
		{
			while (cmp(c, d) <= 0)
			{
				*pts++ = *c++;
			}
			*pts++ = *d++;
		}
		while (c < c_max)
		{
			*pts++ = *c++;
		}
	}

	step2:

	c = array + block * 2;

	c_max = c + block;
	d = c_max;
	d_max = d + block;

	if (cmp(c_max - 1, d_max - 1) <= 0)
	{
		while (c < c_max)
		{
			while (cmp(c, d) > 0)
			{
				*pts++ = *d++;
			}
			*pts++ = *c++;
		}
		while (d < d_max)
		{
			*pts++ = *d++;
		}
	}
	else
	{
		while (d < d_max)
		{
			while (cmp(c, d) <= 0)
			{
				*pts++ = *c++;
			}
			*pts++ = *d++;
		}

		while (c < c_max)
		{
			*pts++ = *c++;
		}
	}

	step3:

	pts = swap;

	c = pts;

	c_max = c + block * 2;

	d = c_max;
	d_max = d + block * 2;

	if (cmp(c_max - 1, d_max - 1) <= 0)
	{
		while (c < c_max)
		{
			while (cmp(c, d) > 0)
			{
				*array++ = *d++;
			}
			*array++ = *c++;
		}
		while (d < d_max)
		{
			*array++ = *d++;
		}
	}
	else
	{
		while (d < d_max)
		{
			while (cmp(d, c) > 0)
			{
				*array++ = *c++;
			}
			*array++ = *d++;
		}
		while (c < c_max)
		{
			*array++ = *c++;
		}
	}
}

void FUNC(quad_merge)(VAR *array, VAR *swap, unsigned int nmemb, unsigned int block, CMPFUNC *cmp)
{
	register VAR *pta, *pte;

	pte = array + nmemb;

	block *= 4;

	while (block * 2 <= nmemb)
	{
		pta = array;

		while (pta + block <= pte)
		{
			FUNC(quad_merge_block)(pta, swap, block, block / 4, cmp);

			pta += block;
		}
		FUNC(tail_merge)(pta, swap, pte - pta, block / 4, cmp);

		block *= 4;
	}
	FUNC(tail_merge)(array, swap, nmemb, block / 4, cmp);
}

// Bottom up merge sort. It copies the right block to swap, next merges
// starting at the tail ends of the two sorted blocks.
// Can be used stand alone. Uses at most nmemb / 2 swap memory.

void FUNC(tail_merge)(VAR *array, VAR *swap, unsigned int nmemb, unsigned int block, CMPFUNC *cmp)
{
	unsigned int offset;
	register VAR *pta, *pts, *c, *c_max, *d, *d_max, *e;

	pts = swap;

	while (block < nmemb)
	{
		for (offset = 0 ; offset + block < nmemb ; offset += block * 2)
		{
			pta = array + offset;

			e = pta + block - 1;

			if (cmp(e, e + 1) <= 0)
			{
				continue;
			}

			if (offset + block * 2 <= nmemb)
			{
				c_max = pts + block;
				d_max = pta + block * 2;
			}
			else
			{
				c_max = pts + nmemb - (offset + block);
				d_max = array + nmemb;
			}

			d = d_max - 1;

			while (cmp(e, d) <= 0)
			{
				d_max--;
				d--;
				c_max--;
			}

			c = pts;
			d = pta + block;

			while (c < c_max)
			{
				*c++ = *d++;
			}
			c--;

			d = pta + block - 1;
			e = d_max - 1;

			if (cmp(pta, pta + block) <= 0)
			{
				*e-- = *d--;

				while (c >= pts)
				{
					while (cmp(d, c) > 0)
					{
						*e-- = *d--;
					}
					*e-- = *c--;
				}
			}
			else
			{
				*e-- = *d--;

				while (d >= pta)
				{
					while (cmp(d, c) <= 0)
					{
						*e-- = *c--;
					}
					*e-- = *d--;
				}
				while (c >= pts)
				{
					*e-- = *c--;
				}
			}
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

void FUNC(quadsort)(void *array, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	if (nmemb < 16)
	{
		FUNC(tail_swap)(array, nmemb, cmp);
	}
	else if (nmemb < 128)
	{
		if (FUNC(quad_swap)(array, nmemb, cmp) == 0)
		{
			VAR swap[64];

			FUNC(tail_merge)(array, swap, nmemb, 16, cmp);
		}
	}
	else if (FUNC(quad_swap)(array, nmemb, cmp) == 0)
	{
		VAR *swap = malloc(nmemb * size / 2);

		if (swap == NULL)
		{
			fprintf(stderr, "quadsort(%p,%zu,%zu,%p): malloc() failed: %s\n", array, nmemb, size, cmp, strerror(errno));

			return;
		}

		FUNC(quad_merge)(array, swap, nmemb, 16, cmp);

		free(swap);
	}
}

void FUNC(quadsort_swap)(VAR *array, VAR *swap, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	if (nmemb < 16)
	{
		FUNC(tail_swap)(array, nmemb, cmp);
	}
	else if (nmemb < 128)
	{
		if (FUNC(quad_swap)(array, nmemb, cmp) == 0)
		{
			FUNC(tail_merge)(array, swap, nmemb, 16, cmp);
		}
	}
	else
	{
		if (FUNC(quad_swap)(array, nmemb, cmp) == 0)
		{
			FUNC(quad_merge)(array, swap, nmemb, 16, cmp);
		}
	}
}
