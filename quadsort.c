/*
	Copyright (C) 2014-2020 Igor van den Hoven ivdhoven@gmail.com
*/

/*
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
	quadsort 1.1.2.1
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>

int counter;
unsigned long long randval;
unsigned char randkey;

typedef int CMPFUNC (const void *a, const void *b);

void quad_sort32(int *array, int *swap, size_t nmemb, size_t block, CMPFUNC *cmp);

void quad_swap32(int *array, int *swap, size_t nmemb, CMPFUNC *cmp)
{
	size_t offset;
	register unsigned char loop;
	register int *pta, *pts, *ptt, tmp;

	pta = array;

	for (offset = 0 ; offset + 4 <= nmemb ; offset += 4)
	{
		if (cmp(&pta[0], &pta[1]) > 0)
		{
			tmp = pta[0];
			pta[0] = pta[1];
			pta[1] = tmp;
		}

		if (cmp(&pta[2], &pta[3]) > 0)
		{
			tmp = pta[2];
			pta[2] = pta[3];
			pta[3] = tmp;
		}

		if (cmp(&pta[1], &pta[2]) > 0)
		{
			if (cmp(&pta[0], &pta[3]) > 0)
			{
				tmp = pta[0];
				pta[0] = pta[2];
				pta[2] = tmp;

				tmp = pta[1];
				pta[1] = pta[3];
				pta[3] = tmp;
			}
			else if (cmp(&pta[0], &pta[2]) <= 0)
			{
				if (cmp(&pta[1], &pta[3]) <= 0)
				{
					tmp = pta[1];
					pta[1] = pta[2];
					pta[2] = tmp;
				}
				else
				{
					tmp = pta[1];
					pta[1] = pta[2];
					pta[2] = pta[3];
					pta[3] = tmp;
				}
			}
			else
			{
				if (cmp(&pta[1], &pta[3]) <= 0)
				{
					tmp = pta[0];
					pta[0] = pta[2];
					pta[2] = pta[1];
					pta[1] = tmp;
				}
				else
				{
					tmp = pta[0];
					pta[0] = pta[2];
					pta[2] = pta[3];
					pta[3] = pta[1];
					pta[1] = tmp;
				}
			}
		}
		pta += 4;
	}

	switch (nmemb - offset)
	{
		case 0:
		case 1:
			break;
		case 2:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0];
				pta[0] = pta[1];
				pta[1] = tmp;
			}
			break;
		case 3:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0];
				pta[0] = pta[1];
				pta[1] = tmp;
			}
			if (cmp(&pta[1], &pta[2]) > 0)
			{
				tmp = pta[1];
				pta[1] = pta[2];
				pta[2] = tmp;
			}
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0];
				pta[0] = pta[1];
				pta[1] = tmp;
			}
			break;
		default:
			assert(nmemb - offset > 3);
	}

	pta = array;

	for (offset = 0 ; offset + 16 <= nmemb ; offset += 16)
	{
		if (cmp(&pta[3], &pta[4]) <= 0)
		{
			if (cmp(&pta[11], &pta[12]) <= 0)
			{
				if (cmp(&pta[7], &pta[8]) <= 0)
				{
					pta += 16;
					continue;
				}
				pts = swap;

				for (loop = 0 ; loop < 16 ; loop++)
					*pts++ = *pta++;

				goto step3;
			}
			pts = swap;

			for (loop = 0 ; loop < 8 ; loop++)
				*pts++ = *pta++;

			goto step2;
		}

		// step1:

		pts = swap;

		if (cmp(&pta[3], &pta[7]) <= 0)
		{
			ptt = pta + 4;

			for (loop = 0 ; loop < 5 ; loop++)
				if (cmp(pta, ptt) > 0)
					*pts++ = *ptt++;
				else
					*pts++ = *pta++;

			while (pta < array + offset + 4)
			{
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			}
			while (ptt < array + offset + 8)
			{
				*pts++ = *ptt++;
			}
			pta = ptt;
		}
		else if (cmp(&pta[0], &pta[7]) > 0)
		{
			if (cmp(&pta[8], &pta[15]) > 0)
			{
				if (cmp(&pta[4], &pta[11]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[12]; pta[12] = tmp;
					tmp = pta[1]; pta[1] = pta[13];	pta[13] = tmp;
					tmp = pta[2]; pta[2] = pta[14]; pta[14] = tmp;
					tmp = pta[3]; pta[3] = pta[15]; pta[15] = tmp;
					tmp = pta[4]; pta[4] = pta[8]; pta[8] = tmp;
					tmp = pta[5]; pta[5] = pta[9]; pta[9] = tmp;
					tmp = pta[6]; pta[6] = pta[10]; pta[10] = tmp;
					tmp = pta[7]; pta[7] = pta[11]; pta[11] = tmp;

					pta += 16;
					continue;
				}
				pta += 4;

				*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

				pta -= 8;

				*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; 

				pta += 8;

				*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

				pta -= 8;

				*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; 

				pta += 4;

				goto step3;
			}
			pta += 4;

			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

			pta -= 8;

			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; 

			pta += 4;
		}
		else
		{
			ptt = pta + 4;

			for (loop = 0 ; loop < 5 ; loop++)
				if (cmp(pta, ptt) > 0)
					*pts++ = *ptt++;
				else
					*pts++ = *pta++;

			while (ptt < array + offset + 8)
			{
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			}
			while (pta < array + offset + 4)
			{
				*pts++ = *pta++;
			}
			pta = ptt;
		}

		step2:

		if (cmp(&pta[3], &pta[7]) <= 0)
		{
			ptt = pta + 4;

			for (loop = 0 ; loop < 4 ; loop++)
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;

			while (pta < array + offset + 12)
			{
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			}
			while (ptt < array + offset + 16)
			{
				*pts++ = *ptt++;
			}
		}
		else if (cmp(&pta[0], &pta[7]) > 0)
		{
			pta += 4;

			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

			pta -= 8;

			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
		}
		else
		{
			ptt = pta + 4;

			for (loop = 0 ; loop < 5 ; loop++)
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;

			while (ptt < array + offset + 16)
			{
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			}
			while (pta < array + offset + 12)
			{
				*pts++ = *pta++;
			}
		}

		step3:

		pta = array + offset;
		pts = swap;

		if (cmp(&pts[7], &pts[15]) <= 0)
		{
			ptt = pts + 8;

			for (loop = 0 ; loop < 8 ; loop++)
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

			while (pts < swap + 8)
			{
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			}
			while (ptt < swap + 16)
			{
				*pta++ = *ptt++;
			}
		}
		else if (cmp(&pts[0], &pts[15]) > 0)
		{
			pts += 8;

			for (loop = 0 ; loop < 8 ; loop++)
				*pta++ = *pts++;

			pts -= 16;

			for (loop = 0 ; loop < 8 ; loop++)
				*pta++ = *pts++;
		}
		else
		{
			ptt = pts + 8;

			for (loop = 0 ; loop < 9 ; loop++)
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

			while (ptt < swap + 16)
			{
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			}
			while (pts < swap + 8)
			{
				*pta++ = *pts++;
			}
		}
	}

	switch (nmemb - offset)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			return;
		default:
			quad_sort32(pta, swap, nmemb - offset, 4, cmp);
	}
}

void quad_sort64(long long *array, long long *swap, size_t nmemb, size_t block, CMPFUNC *cmp);

void quad_swap64(long long *array, long long *swap, size_t nmemb, CMPFUNC *cmp)
{
	size_t offset;
	register long long *pta, *pts, *ptt, tmp;

	pta = array;

	for (offset = 0 ; offset + 4 <= nmemb ; offset += 4)
	{
		if (cmp(&pta[0], &pta[1]) > 0)
		{
			tmp = pta[0];
			pta[0] = pta[1];
			pta[1] = tmp;
		}

		if (cmp(&pta[2], &pta[3]) > 0)
		{
			tmp = pta[2];
			pta[2] = pta[3];
			pta[3] = tmp;
		}

		if (cmp(&pta[1], &pta[2]) > 0)
		{
			if (cmp(&pta[0], &pta[3]) > 0)
			{
				tmp = pta[0];
				pta[0] = pta[2];
				pta[2] = tmp;

				tmp = pta[1];
				pta[1] = pta[3];
				pta[3] = tmp;
			}
			else if (cmp(&pta[0], &pta[2]) <= 0)
			{
				if (cmp(&pta[1], &pta[3]) <= 0)
				{
					tmp = pta[1];
					pta[1] = pta[2];
					pta[2] = tmp;
				}
				else
				{
					tmp = pta[1];
					pta[1] = pta[2];
					pta[2] = pta[3];
					pta[3] = tmp;
				}
			}
			else
			{
				if (cmp(&pta[1], &pta[3]) <= 0)
				{
					tmp = pta[0];
					pta[0] = pta[2];
					pta[2] = pta[1];
					pta[1] = tmp;
				}
				else
				{
					tmp = pta[0];
					pta[0] = pta[2];
					pta[2] = pta[3];
					pta[3] = pta[1];
					pta[1] = tmp;
				}
			}
		}
		pta += 4;
	}

	switch (nmemb - offset)
	{
		case 0:
		case 1:
			break;
		case 2:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0];
				pta[0] = pta[1];
				pta[1] = tmp;
			}
			break;
		case 3:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0];
				pta[0] = pta[1];
				pta[1] = tmp;
			}
			if (cmp(&pta[1], &pta[2]) > 0)
			{
				tmp = pta[1];
				pta[1] = pta[2];
				pta[2] = tmp;
			}
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0];
				pta[0] = pta[1];
				pta[1] = tmp;
			}
			break;
		default:
			assert(nmemb - offset > 3);
	}

	pta = array;

	for (offset = 0 ; offset + 16 <= nmemb ; offset += 16)
	{
		if (cmp(&pta[3], &pta[4]) <= 0)
		{
			if (cmp(&pta[11], &pta[12]) <= 0)
			{
				if (cmp(&pta[7], &pta[8]) <= 0)
				{
					pta += 16;
					continue;
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

			while (pta < array + offset + 4)
			{
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			}
			while (ptt < array + offset + 8)
			{
				*pts++ = *ptt++;
			}
			pta = ptt;
		}
		else if (cmp(&pta[0], &pta[7]) > 0)
		{
			if (cmp(&pta[8], &pta[15]) > 0)
			{
				if (cmp(&pta[4], &pta[11]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[12]; pta[12] = tmp;
					tmp = pta[1]; pta[1] = pta[13];	pta[13] = tmp;
					tmp = pta[2]; pta[2] = pta[14]; pta[14] = tmp;
					tmp = pta[3]; pta[3] = pta[15]; pta[15] = tmp;
					tmp = pta[4]; pta[4] = pta[8]; pta[8] = tmp;
					tmp = pta[5]; pta[5] = pta[9]; pta[9] = tmp;
					tmp = pta[6]; pta[6] = pta[10]; pta[10] = tmp;
					tmp = pta[7]; pta[7] = pta[11]; pta[11] = tmp;

					pta += 16;
					continue;
				}
				pta += 4;

				*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

				pta -= 8;

				*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; 

				pta += 8;

				*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

				pta -= 8;

				*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; 

				pta += 4;

				goto step3;
			}
			pta += 4;

			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

			pta -= 8;

			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; 

			pta += 4;
		}
		else
		{
			ptt = pta + 4;

			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;

			while (ptt < array + offset + 8)
			{
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			}
			while (pta < array + offset + 4)
			{
				*pts++ = *pta++;
			}
			pta = ptt;
		}

		step2:

		if (cmp(&pta[3], &pta[7]) <= 0)
		{
			ptt = pta + 4;

			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
//			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;

			while (pta < array + offset + 12)
			{
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			}
			while (ptt < array + offset + 16)
			{
				*pts++ = *ptt++;
			}
		}
		else if (cmp(&pta[0], &pta[7]) > 0)
		{
			pta += 4;

			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

			pta -= 8;

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

			while (ptt < array + offset + 16)
			{
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			}
			while (pta < array + offset + 12)
			{
				*pts++ = *pta++;
			}
		}

		step3:

		pta = array + offset;
		pts = swap;

		if (cmp(&pts[7], &pts[15]) <= 0)
		{
			ptt = pts + 8;

			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
//			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

			while (pts < swap + 8)
			{
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			}
			while (ptt < swap + 16)
			{
				*pta++ = *ptt++;
			}
		}
		else if (cmp(&pts[0], &pts[15]) > 0)
		{
			pts += 8;

			*pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++;
			*pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++;

			pts -= 16;

			*pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++;
			*pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++;
		}
		else
		{
			ptt = pts + 8;

			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

			while (ptt < swap + 16)
			{
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			}
			while (pts < swap + 8)
			{
				*pta++ = *pts++;
			}
		}
	}

	switch (nmemb - offset)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			return;
		default:
			quad_sort64(pta, swap, nmemb - offset, 4, cmp);
	}
}

void quad_sort32(int *array, int *swap, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	size_t offset;
	register int *pta, *pts, *c, *c_max, *d, *d_max;

	while (block < nmemb)
	{
		offset = 0;

		while (offset + block < nmemb)
		{
			pta = array;
			pta += offset;

			c_max = pta + block;

			if (cmp(c_max - 1, c_max) <= 0)
			{
				if (offset + block * 3 < nmemb)
				{
					c_max = pta + block * 3;

					if (cmp(c_max - 1, c_max) <= 0)
					{
						c_max = pta + block * 2;

						if (cmp(c_max - 1, c_max) <= 0)
						{
							offset += block * 4;
							continue;
						}
						pts = swap;

						c = pta;

						while (c < c_max - 8)
						{
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						}
						while (c < c_max)
							*pts++ = *c++;

						c = c_max;
						c_max = offset + block * 4 <= nmemb ? c + block * 2 : array + nmemb;

						while (c < c_max - 8)
						{
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						}
						while (c < c_max)
							*pts++ = *c++;

						goto step3;
					}
					pts = swap;

					c = pta;
					c_max = pta + block * 2;

					while (c < c_max)
						*pts++ = *c++;

					goto step2;
				}
				else if (offset + block * 2 < nmemb)
				{
					c_max = pta + block * 2;

					if (cmp(c_max - 1, c_max) <= 0)
					{
						offset += block * 4;
						continue;
					}
					pts = swap;

					c = pta;

					while (c < c_max - 8)
					{
						*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
					}
					while (c < c_max)
						*pts++ = *c++;

					goto step2;
				}
				else
				{
					offset += block * 4;
					continue;
				}
			}

			// step1:

			pts = swap;

			c = pta;

			d = c_max;
			d_max = offset + block * 2 <= nmemb ? d + block : array + nmemb;

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
					*pts++ = *d++;
			}
			else if (cmp(c, d_max - 1) > 0)
			{
				if (offset + block * 4 <= nmemb)
				{
					int *e, *e_max, *f, *f_max, tmp;

					e = pta + block * 2;
					e_max = e + block;
					f = e_max;
					f_max = f + block;

					if (cmp(e, f_max - 1) > 0)
					{
						if (cmp(d, f_max - 1) > 0)
						{
							while (c < c_max)
							{
								tmp = *c;
								*c++ = *f;
								*f++ = tmp;
							}
							while (d < d_max)
							{
								tmp = *d;
								*d++ = *e;
								*e++ = tmp;
							}
							offset += block * 4;
							continue;
						}
					}
				}

				while (d < d_max - 8)
				{
					*pts++ = *d++; *pts++ = *d++; *pts++ = *d++; *pts++ = *d++;
					*pts++ = *d++; *pts++ = *d++; *pts++ = *d++; *pts++ = *d++;
				}
				while (d < d_max)
					*pts++ = *d++;

				while (c < c_max - 8)
				{
					*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
					*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
				}
				while (c < c_max)
					*pts++ = *c++;
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
					*pts++ = *c++;
			}

			step2:

			if (offset + block * 2 < nmemb)
			{
				c = pta + block * 2;

				if (offset + block * 3 < nmemb)
				{
					c_max = c + block;
					d = c_max;
					d_max = offset + block * 4 <= nmemb ? d + block : array + nmemb;

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
							*pts++ = *d++;
					}
					else if (cmp(c, d_max - 1) > 0)
					{
						while (d < d_max - 8)
						{
							*pts++ = *d++; *pts++ = *d++; *pts++ = *d++; *pts++ = *d++;
							*pts++ = *d++; *pts++ = *d++; *pts++ = *d++; *pts++ = *d++;
						}
						while (d < d_max)
							*pts++ = *d++;

						while (c < c_max - 8)
						{
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						}
						while (c < c_max)
							*pts++ = *c++;
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
							*pts++ = *c++;
					}
				}
				else
				{
					d = c;
					d_max = array + nmemb;

					pts = swap;
					c = pts;
					c_max = c + block * 2;

					goto quickstep;
				}
			}

			step3:

			pts = swap;

			c = pts;

			if (offset + block * 2 < nmemb)
			{
				c_max = c + block * 2;

				d = c_max;
				d_max = offset + block * 4 <= nmemb ? d + block * 2 : pts + nmemb - offset;

				quickstep:

				if (cmp(c_max - 1, d_max - 1) <= 0)
				{
					while (c < c_max)
					{
						while (cmp(c, d) > 0)
						{
							*pta++ = *d++;
						}
						*pta++ = *c++;
					}

					while (d < d_max)
						*pta++ = *d++;
				}
				else if (cmp(c, d_max - 1) > 0)
				{
					while (d < d_max - 16)
					{
						*pta++ = *d++; *pta++ = *d++; *pta++ = *d++; *pta++ = *d++;
						*pta++ = *d++; *pta++ = *d++; *pta++ = *d++; *pta++ = *d++;
						*pta++ = *d++; *pta++ = *d++; *pta++ = *d++; *pta++ = *d++;
						*pta++ = *d++; *pta++ = *d++; *pta++ = *d++; *pta++ = *d++;
					}
					while (d < d_max)
						*pta++ = *d++;

					while (c < c_max - 16)
					{
						*pta++ = *c++; *pta++ = *c++; *pta++ = *c++; *pta++ = *c++;
						*pta++ = *c++; *pta++ = *c++; *pta++ = *c++; *pta++ = *c++;
						*pta++ = *c++; *pta++ = *c++; *pta++ = *c++; *pta++ = *c++;
						*pta++ = *c++; *pta++ = *c++; *pta++ = *c++; *pta++ = *c++;
					}
					while (c < c_max)
						*pta++ = *c++;
				}
				else
				{
					while (d < d_max)
					{
						while (cmp(d, c) > 0)
						{
							*pta++ = *c++;
						}
						*pta++ = *d++;
					}

					while (c < c_max)
						*pta++ = *c++;
				}
			}
			else
			{
				d_max = pts + nmemb - offset;

				while (c < d_max - 8)
				{
					*pta++ = *c++; *pta++ = *c++; *pta++ = *c++; *pta++ = *c++;
					*pta++ = *c++; *pta++ = *c++; *pta++ = *c++; *pta++ = *c++;
				}
				while (c < d_max)
					*pta++ = *c++;
			}
			offset += block * 4;
		}
		block *= 4;
	}
}

void quad_sort64(long long *array, long long *swap, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	size_t offset;
	register long long *pta, *pts, *c, *c_max, *d, *d_max;

	while (block < nmemb)
	{
		offset = 0;

		while (offset + block < nmemb)
		{
			pta = array;
			pta += offset;

			c_max = pta + block;

			if (cmp(c_max - 1, c_max) <= 0)
			{
				if (offset + block * 3 < nmemb)
				{
					c_max = pta + block * 3;

					if (cmp(c_max - 1, c_max) <= 0)
					{
						c_max = pta + block * 2;

						if (cmp(c_max - 1, c_max) <= 0)
						{
							offset += block * 4;
							continue;
						}
						pts = swap;

						c = pta;

						while (c < c_max - 8)
						{
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						}
						while (c < c_max)
							*pts++ = *c++;

						c = c_max;
						c_max = offset + block * 4 <= nmemb ? c + block * 2 : array + nmemb;

						while (c < c_max - 8)
						{
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						}
						while (c < c_max)
							*pts++ = *c++;

						goto step3;
					}
					pts = swap;

					c = pta;
					c_max = pta + block * 2;

					while (c < c_max)
						*pts++ = *c++;

					goto step2;
				}
				else if (offset + block * 2 < nmemb)
				{
					c_max = pta + block * 2;

					if (cmp(c_max - 1, c_max) <= 0)
					{
						offset += block * 4;
						continue;
					}
					pts = swap;

					c = pta;

					while (c < c_max - 8)
					{
						*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
					}
					while (c < c_max)
						*pts++ = *c++;

					goto step2;
				}
				else
				{
					offset += block * 4;
					continue;
				}
			}

			// step1:

			pts = swap;

			c = pta;

			d = c_max;
			d_max = offset + block * 2 <= nmemb ? d + block : array + nmemb;

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
					*pts++ = *d++;
			}
			else if (cmp(c, d_max - 1) > 0)
			{
				if (offset + block * 4 <= nmemb)
				{
					long long *e, *e_max, *f, *f_max, tmp;

					e = pta + block * 2;
					e_max = e + block;
					f = e_max;
					f_max = f + block;

					if (cmp(e, f_max - 1) > 0)
					{
						if (cmp(d, f_max - 1) > 0)
						{
							while (c < c_max)
							{
								tmp = *c;
								*c++ = *f;
								*f++ = tmp;
							}
							while (d < d_max)
							{
								tmp = *d;
								*d++ = *e;
								*e++ = tmp;
							}
							offset += block * 4;
							continue;
						}
					}
				}

				while (d < d_max - 8)
				{
					*pts++ = *d++; *pts++ = *d++; *pts++ = *d++; *pts++ = *d++;
					*pts++ = *d++; *pts++ = *d++; *pts++ = *d++; *pts++ = *d++;
				}
				while (d < d_max)
					*pts++ = *d++;

				while (c < c_max - 8)
				{
					*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
					*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
				}
				while (c < c_max)
					*pts++ = *c++;
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
					*pts++ = *c++;
			}

			step2:

			if (offset + block * 2 < nmemb)
			{
				c = pta + block * 2;

				if (offset + block * 3 < nmemb)
				{
					c_max = c + block;
					d = c_max;
					d_max = offset + block * 4 <= nmemb ? d + block : array + nmemb;

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
							*pts++ = *d++;
					}
					else if (cmp(c, d_max - 1) > 0)
					{
						while (d < d_max - 8)
						{
							*pts++ = *d++; *pts++ = *d++; *pts++ = *d++; *pts++ = *d++;
							*pts++ = *d++; *pts++ = *d++; *pts++ = *d++; *pts++ = *d++;
						}
						while (d < d_max)
							*pts++ = *d++;

						while (c < c_max - 8)
						{
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						}
						while (c < c_max)
							*pts++ = *c++;
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
							*pts++ = *c++;
					}
				}
				else
				{
					d = c;
					d_max = array + nmemb;

					pts = swap;
					c = pts;
					c_max = c + block * 2;

					goto quickstep;
				}
			}

			step3:

			pts = swap;

			c = pts;

			if (offset + block * 2 < nmemb)
			{
				c_max = c + block * 2;

				d = c_max;
				d_max = offset + block * 4 <= nmemb ? d + block * 2 : pts + nmemb - offset;

				quickstep:

				if (cmp(c_max - 1, d_max - 1) <= 0)
				{
					while (c < c_max)
					{
						while (cmp(c, d) > 0)
						{
							*pta++ = *d++;
						}
						*pta++ = *c++;
					}

					while (d < d_max)
						*pta++ = *d++;
				}
				else if (cmp(c, d_max - 1) > 0)
				{
					while (d < d_max - 16)
					{
						*pta++ = *d++; *pta++ = *d++; *pta++ = *d++; *pta++ = *d++;
						*pta++ = *d++; *pta++ = *d++; *pta++ = *d++; *pta++ = *d++;
						*pta++ = *d++; *pta++ = *d++; *pta++ = *d++; *pta++ = *d++;
						*pta++ = *d++; *pta++ = *d++; *pta++ = *d++; *pta++ = *d++;
					}
					while (d < d_max)
						*pta++ = *d++;

					while (c < c_max - 16)
					{
						*pta++ = *c++; *pta++ = *c++; *pta++ = *c++; *pta++ = *c++;
						*pta++ = *c++; *pta++ = *c++; *pta++ = *c++; *pta++ = *c++;
						*pta++ = *c++; *pta++ = *c++; *pta++ = *c++; *pta++ = *c++;
						*pta++ = *c++; *pta++ = *c++; *pta++ = *c++; *pta++ = *c++;
					}
					while (c < c_max)
						*pta++ = *c++;
				}
				else
				{
					while (d < d_max)
					{
						while (cmp(d, c) > 0)
						{
							*pta++ = *c++;
						}
						*pta++ = *d++;
					}

					while (c < c_max)
						*pta++ = *c++;
				}
			}
			else
			{
				d_max = pts + nmemb - offset;

				while (c < d_max - 8)
				{
					*pta++ = *c++; *pta++ = *c++; *pta++ = *c++; *pta++ = *c++;
					*pta++ = *c++; *pta++ = *c++; *pta++ = *c++; *pta++ = *c++;
				}
				while (c < d_max)
					*pta++ = *c++;
			}
			offset += block * 4;
		}
		block *= 4;
	}
}

void quadsort(void *array, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	void *swap;

	swap = malloc(nmemb * size);

	if (size == sizeof(int))
	{
		quad_swap32(array, swap, nmemb, cmp);
		quad_sort32(array, swap, nmemb, 16, cmp);
	}
	else if (size == sizeof(long long))
	{
		quad_swap64(array, swap, nmemb, cmp);
		quad_sort64(array, swap, nmemb, 16, cmp);
	}
	else
	{
		assert(size == 4 || size == 8);
	}

	free(swap);
}

// Must prevent inlining so the benchmark is fair.
// Remove __attribute__ ((noinline)) for full throttle.

__attribute__ ((noinline)) int cmp_int(const void * a, const void * b)
{
	counter++;

	return *(int *) a - *(int *) b;
}

int cmp_str(const void * a, const void * b)
{
	return strcmp(*(const char **) a, *(const char **) b);
}

int cmp_float(const void * a, const void * b)
{
	return *(float *) a - *(float *) b;
}

// benchmarking utilities

long long utime()
{
	struct timeval now_time;

	gettimeofday(&now_time, NULL);

	return now_time.tv_sec * 1000000LL + now_time.tv_usec;
}

void seed_rand(unsigned long long seed)
{
	srand(seed);

	randval = randval % 1000000000ULL;
	randkey = randval % 2;
}

// 2305019424311111437ULL
// 1062272168593625449
// 26196137413795067
   
int generate_rand()
{
	static unsigned long long primes[] = {26196137413795067, 1062272168593625449 };

	return rand();

	randval = randval + primes[0];

	return randval % 1000000000ULL;
}

void test_quad(int *z_array, int *r_array, int minimum, int maximum, int samples, int repetitions, const char *desc)
{
	long long start, end, total, best;
	int cnt, rep, sam, max;

	best = 0;

	for (sam = 0 ; sam <= samples ; sam++)
	{
		total = 0;
		counter = 0;

		max = minimum;

		for (rep = 0 ; rep < repetitions ; rep++)
		{
			memcpy(z_array, r_array, max * sizeof(int));

			if (max == 10 && sam == 0 && rep == 0)
				printf("\e[1;31m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n",
					z_array[0], z_array[1], z_array[2], z_array[3], z_array[4],
					z_array[5], z_array[6], z_array[7], z_array[8], z_array[9]);

			start = utime();

			quadsort(z_array, max, sizeof(int), cmp_int);

			end = utime();

			if (max == 10 && sam == 0 && rep == 0)
				printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n",
					z_array[0], z_array[1], z_array[2], z_array[3], z_array[4],
					z_array[5], z_array[6], z_array[7], z_array[8], z_array[9]);

			total += end - start;

			if (max < maximum)
			{
				max++;
			}

		}

		if (!best || total < best)
		{
			best = total;
		}
	}
	if (max == 1000 || max == 1000000)
	{
		printf("         quadsort: sorted %7d ints in %f seconds. O(%8d) (%s)\n", maximum, best / 1000000.0, counter, desc);
	}
	else
	{
		printf("         quadsort: sorted %7d ints in %f seconds. (%s)\n", maximum, best / 1000000.0, desc);
	}
}

void test_quick(int *z_array, int *r_array, int minimum, int maximum, int samples, int repetitions, const char *desc)
{
	long long start, end, total, best;
	int cnt, sam, rep, max;

	best = 0;

	for (sam = 0 ; sam < samples ; sam++)
	{
		total = 0;
		counter = 0;

		max = minimum;

		for (rep = 0 ; rep < repetitions ; rep++)
		{
			memcpy(z_array, r_array, max * sizeof(int));

			if (maximum == 10 && sam == 0 && rep == 0) printf("\e[1;31m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n", z_array[0], z_array[1], z_array[2], z_array[3], z_array[4], z_array[5], z_array[6], z_array[7], z_array[8], z_array[9]);

			start = utime();

			qsort(z_array, max, sizeof(int), cmp_int);

			end = utime();

			if (maximum == 10 && sam == 0 && rep == 0) printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n", z_array[0], z_array[1], z_array[2], z_array[3], z_array[4], z_array[5], z_array[6], z_array[7], z_array[8], z_array[9]);

			total += end - start;

			if (max < maximum)
			{
				max++;
			}

		}
		if (!best || total < best)
		{
			best = total;
		}
	}
	if (max == 1000 || max == 1000000)
	{
		printf("            qsort: sorted %7d ints in %f seconds. O(%8d) (%s)\n", maximum, best / 1000000.0, counter, desc);
	}
	else
	{
		printf("            qsort: sorted %7d ints in %f seconds. (%s)\n", maximum, best / 1000000.0, desc);
	}
}

void repeater(int *a_array, size_t max)
{
	size_t cnt, tally = 0;

	for (cnt = 1 ; cnt < max ; cnt++)
	{
		if (a_array[cnt - 1] == a_array[cnt])
		{
//			printf("%03d repeat value %d\n", ++tally, a_array[cnt]);
		}
	}
}
	
void validate(int *a_array, int *b_array, size_t max)
{
	size_t cnt;

	for (cnt = 1 ; cnt < max ; cnt++)
	{

		if (a_array[cnt - 1] > a_array[cnt])
		{
			printf("         quadsort: not properly sorted at index %lu. (%d vs %d\n", cnt, b_array[cnt - 1], b_array[cnt]);
			break;
		}
	}

	for (cnt = 1 ; cnt < max ; cnt++)
	{
		if (b_array[cnt - 1] > b_array[cnt])
		{
			printf("            qsort: not properly sorted at index %lu. (%d vs %d\n", cnt, b_array[cnt - 1], b_array[cnt]);
			break;
		}
	}

	for (cnt = 1 ; cnt < max ; cnt++)
	{
		if (a_array[cnt] != b_array[cnt])
		{
			printf("         validate: a_array != b_array at index %lu. (%d vs %d\n", cnt, a_array[cnt], b_array[cnt]);
			break;
		}
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	size_t max = 100000;
	size_t samples = 100;
	size_t repetitions = 1;

	size_t cnt, rnd;
	int *a_array, *b_array, *r_array;

	if (argc >= 1 && argv[1] && *argv[1])
	{
		max = atoi(argv[1]);
	}

	if (argc >= 2 && argv[2] && *argv[2])
	{
		samples = atoi(argv[2]);
	}

	if (argc >= 3 && argv[3] && *argv[3])
	{
		repetitions = atoi(argv[3]);
	}

	rnd = 1;

	seed_rand(rnd);

	a_array = malloc(max * sizeof(int));
	b_array = malloc(max * sizeof(int));
	r_array = malloc(max * sizeof(int));

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = (int) generate_rand();
	}

	test_quad(a_array, r_array, max, max, samples, repetitions, "random order");

	test_quick(b_array, r_array, max, max, samples, repetitions, "random order");

	validate(a_array, b_array, max);

	repeater(a_array, max);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = cnt;
	}

	test_quad(a_array, r_array, max, max, samples, repetitions, "forward order");

	test_quick(b_array, r_array, max, max, samples, repetitions, "forward order");

	validate(a_array, b_array, max);

/*
	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = 1;
	}

	test_quad(a_array, r_array, max, max, samples, repetitions, "repeat order");

	test_quick(b_array, r_array, max, max, samples, repetitions, "repeat order");

	validate(a_array, b_array, max);
*/

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = max - cnt;
	}

	test_quad(a_array, r_array, max, max, samples, repetitions, "reverse order");
	test_quick(b_array, r_array, max, max, samples, repetitions, "reverse order");

	validate(a_array, b_array, max);

	// wave order

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = cnt + (cnt % 2 ? 16777216 : 33554432);
	}

	test_quad(a_array, r_array, max, max, samples, repetitions, "wave order");
	test_quick(b_array, r_array, max, max, samples, repetitions, "wave order");

	validate(a_array, b_array, max);

	// random tail

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max * 3 / 4 ; cnt++)
	{
		r_array[cnt] = cnt;
	}

	for (cnt = max * 3 / 4 ; cnt < max ; cnt++)
	{
		r_array[cnt] = generate_rand();
	}

	test_quad(a_array, r_array, max, max, samples, repetitions, "random tail");

	test_quick(b_array, r_array, max, max, samples, repetitions, "random tail");

	validate(a_array, b_array, max);

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = generate_rand();
	}

	if (samples == 100)
	{
		test_quad(a_array, r_array, 0, 1024, 1, 1025, "random range");

		test_quick(b_array, r_array, 0, 1024, 1, 1025, "random range");

		validate(a_array, b_array, 1000);
	}
	free(a_array);
	free(b_array);
	free(r_array);

	return 0;
}
