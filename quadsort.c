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
	quadsort 1.1.1.3
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>


typedef int CMPFUNC (const void *a, const void *b);

void quad_swap32(int *array, size_t nmemb, CMPFUNC *cmp)
{
	size_t offset;
	register int *pta, pts;

	pta = array;

	for (offset = 0 ; offset + 4 <= nmemb ; offset += 4)
	{
		if (cmp(&pta[0], &pta[1]) > 0)
		{
			pts = pta[0];
			pta[0] = pta[1];
			pta[1] = pts;
		}

		if (cmp(&pta[2], &pta[3]) > 0)
		{
			pts = pta[2];
			pta[2] = pta[3];
			pta[3] = pts;
		}

		if (cmp(&pta[1], &pta[2]) > 0)
		{
			if (cmp(&pta[0], &pta[3]) > 0)
			{
				pts = pta[0];
				pta[0] = pta[2];
				pta[2] = pts;

				pts = pta[1];
				pta[1] = pta[3];
				pta[3] = pts;
			}
			else if (cmp(&pta[0], &pta[2]) <= 0)
			{
				if (cmp(&pta[1], &pta[3]) <= 0)
				{
					pts = pta[1];
					pta[1] = pta[2];
					pta[2] = pts;
				}
				else
				{
					pts = pta[1];
					pta[1] = pta[2];
					pta[2] = pta[3];
					pta[3] = pts;
				}
			}
			else
			{
				if (cmp(&pta[1], &pta[3]) <= 0)
				{
					pts = pta[0];
					pta[0] = pta[2];
					pta[2] = pta[1];
					pta[1] = pts;
				}
				else
				{
					pts = pta[0];
					pta[0] = pta[2];
					pta[2] = pta[3];
					pta[3] = pta[1];
					pta[1] = pts;
				}
			}
		}
		pta += 4;
	}

	switch (nmemb - offset)
	{
		case 0:
		case 1:
			return;
		case 2:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts = pta[0];
				pta[0] = pta[1];
				pta[1] = pts;
			}
			return;
		case 3:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts = pta[0];
				pta[0] = pta[1];
				pta[1] = pts;
			}
			if (cmp(&pta[1], &pta[2]) > 0)
			{
				pts = pta[1];
				pta[1] = pta[2];
				pta[2] = pts;
			}
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts = pta[0];
				pta[0] = pta[1];
				pta[1] = pts;
			}
			return;
		default:
			assert(nmemb - offset > 3);
	}
}

void quad_swap64(void *array, size_t nmemb, CMPFUNC *cmp)
{
	size_t offset;
	register long long *pta, pts;

	pta = array;

	for (offset = 0 ; offset + 4 <= nmemb ; offset += 4)
	{
		if (cmp(&pta[0], &pta[1]) > 0)
		{
			pts = pta[0];
			pta[0] = pta[1];
			pta[1] = pts;
		}

		if (cmp(&pta[2], &pta[3]) > 0)
		{
			pts = pta[2];
			pta[2] = pta[3];
			pta[3] = pts;
		}

		if (cmp(&pta[1], &pta[2]) > 0)
		{
			if (cmp(&pta[0], &pta[3]) > 0)
			{
				pts = pta[0];
				pta[0] = pta[2];
				pta[2] = pts;

				pts = pta[1];
				pta[1] = pta[3];
				pta[3] = pts;
			}
			else if (cmp(&pta[0], &pta[2]) <= 0)
			{
				if (cmp(&pta[1], &pta[3]) <= 0)
				{
					pts = pta[1];
					pta[1] = pta[2];
					pta[2] = pts;
				}
				else
				{
					pts = pta[1];
					pta[1] = pta[2];
					pta[2] = pta[3];
					pta[3] = pts;
				}
			}
			else
			{
				if (cmp(&pta[1], &pta[3]) <= 0)
				{
					pts = pta[0];
					pta[0] = pta[2];
					pta[2] = pta[1];
					pta[1] = pts;
				}
				else
				{
					pts = pta[0];
					pta[0] = pta[2];
					pta[2] = pta[3];
					pta[3] = pta[1];
					pta[1] = pts;
				}
			}
		}
		pta += 4;
	}

	switch (nmemb - offset)
	{
		case 0:
		case 1:
			return;
		case 2:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts = pta[0];
				pta[0] = pta[1];
				pta[1] = pts;
			}
			return;
		case 3:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts = pta[0];
				pta[0] = pta[1];
				pta[1] = pts;
			}
			if (cmp(&pta[1], &pta[2]) > 0)
			{
				pts = pta[1];
				pta[1] = pta[2];
				pta[2] = pts;
			}
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts = pta[0];
				pta[0] = pta[1];
				pta[1] = pts;
			}
			return;
		default:
			assert(nmemb - offset > 3);
	}
}

void quad_sort32(int *array, int *swap, size_t nmemb, CMPFUNC *cmp)
{
	size_t offset, block = 4;
	register int *pta, *pts, *c, *c_max, *d, *d_max, *end;

	end = array;
	end += nmemb;

	while (block < nmemb)
	{
		offset = 0;

		while (offset + block < nmemb)
		{
			pta = array;
			pta += offset;

			d_max = pta + block;

			if (cmp(d_max - 1, d_max) <= 0)
			{
				if (offset + block * 3 < nmemb)
				{
					d_max = pta + block * 3;

					if (cmp(d_max - 1, d_max) <= 0)
					{
						d_max = pta + block * 2;

						if (cmp(d_max - 1, d_max) <= 0)
						{
							offset += block * 4;
							continue;
						}
						pts = swap;

						c = pta;
						c_max = pta + block * 2;
						d = c_max;
						d_max = offset + block * 4 <= nmemb ? d + block * 2 : end;

						while (c < c_max - 8)
						{
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						}
						while (c < c_max)
							*pts++ = *c++;

						while (d < d_max - 8)
						{
							*pts++ = *d++; *pts++ = *d++; *pts++ = *d++; *pts++ = *d++;
							*pts++ = *d++; *pts++ = *d++; *pts++ = *d++; *pts++ = *d++;
						}
						while (d < d_max)
							*pts++ = *d++;

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
					d_max = pta + block * 2;

					if (cmp(d_max - 1, d_max) <= 0)
					{
						offset += block * 4;
						continue;
					}
					pts = swap;

					c = pta;
					c_max = pta + block * 2;

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
			c_max = pta + block;

			d = c_max;
			d_max = offset + block * 2 <= nmemb ? d + block : end;

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

			step2:

			if (offset + block * 2 < nmemb)
			{
				c = pta + block * 2;

				if (offset + block * 3 < nmemb)
				{
					c_max = c + block;
					d = c_max;
					d_max = offset + block * 4 <= nmemb ? d + block : end;

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
					d_max = end;

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

void quad_sort64(void *array, void *swap, size_t nmemb, CMPFUNC *cmp)
{
	size_t offset, block = 4;
	register long long *pta, *pts, *c, *c_max, *d, *d_max, *end;

	end = array;
	end += nmemb;

	while (block < nmemb)
	{
		offset = 0;

		while (offset + block < nmemb)
		{
			pta = array;
			pta += offset;

			d_max = pta + block;

			if (cmp(d_max - 1, d_max) <= 0)
			{
				if (offset + block * 3 < nmemb)
				{
					d_max = pta + block * 3;

					if (cmp(d_max - 1, d_max) <= 0)
					{
						d_max = pta + block * 2;

						if (cmp(d_max - 1, d_max) <= 0)
						{
							offset += block * 4;
							continue;
						}
						pts = swap;

						c = pta;
						c_max = pta + block * 2;
						d = c_max;
						d_max = offset + block * 4 <= nmemb ? d + block * 2 : end;

						while (c < c_max - 8)
						{
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
							*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						}
						while (c < c_max)
							*pts++ = *c++;

						while (d < d_max - 8)
						{
							*pts++ = *d++; *pts++ = *d++; *pts++ = *d++; *pts++ = *d++;
							*pts++ = *d++; *pts++ = *d++; *pts++ = *d++; *pts++ = *d++;
						}
						while (d < d_max)
							*pts++ = *d++;

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
					d_max = pta + block * 2;

					if (cmp(d_max - 1, d_max) <= 0)
					{
						offset += block * 4;
						continue;
					}
					pts = swap;

					c = pta;
					c_max = pta + block * 2;

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
			c_max = pta + block;

			d = c_max;
			d_max = offset + block * 2 <= nmemb ? d + block : end;

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

			step2:

			if (offset + block * 2 < nmemb)
			{
				c = pta + block * 2;

				if (offset + block * 3 < nmemb)
				{
					c_max = c + block;
					d = c_max;
					d_max = offset + block * 4 <= nmemb ? d + block : end;

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
					d_max = end;

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
		quad_swap32(array, nmemb, cmp);

		quad_sort32(array, swap, nmemb, cmp);
	}
	else if (size == sizeof(long long))
	{
		quad_swap64(array, nmemb, cmp);

		quad_sort64(array, swap, nmemb, cmp);
	}
	else
	{
		assert(size == 4 || size == 8);
	}

	free(swap);
}

// Must prevent inlining so the benchmark is fair. Remove for full throttle.

__attribute__ ((noinline)) int cmp_int(const void * a, const void * b)
{
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

void test_quad(int *z_array, int *r_array, int max, char *desc)
{
	long long start, end, total, best;
	int cnt, sample;

	best = 0;

	for (sample = 0 ; sample < 100 ; sample++)
	{
		memcpy(z_array, r_array, max * sizeof(int));

		if (sample == 0 && max == 10) printf("\e[1;31m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n", z_array[0], z_array[1], z_array[2], z_array[3], z_array[4], z_array[5], z_array[6], z_array[7], z_array[8], z_array[9]);

		start = utime();

		quadsort(z_array, max, sizeof(int), cmp_int);

		end = utime();

		if (sample == 0 && max == 10) printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n", z_array[0], z_array[1], z_array[2], z_array[3], z_array[4], z_array[5], z_array[6], z_array[7], z_array[8], z_array[9]);

		total = end - start;

		if (!best || total < best)
		{
			best = total;
		}
	}
	printf("         quadsort: sorted %d elements in %f seconds. (%s)\n", max, best / 1000000.0, desc);

	for (cnt = 1 ; cnt < max ; cnt++)
	{
		if (z_array[cnt - 1] > z_array[cnt])
		{
			printf("         quadsort: not properly sorted at index %d. (%d vs %d\n", cnt, z_array[cnt - 1], z_array[cnt]);
			break;
		}
	}
}

void test_quick(int *z_array, int *r_array, int max, char *desc)
{
	long long start, end, total, best;
	int cnt, sample;

	best = 0;

	for (sample = 0 ; sample < 100 ; sample++)
	{
		memcpy(z_array, r_array, max * sizeof(int));

		if (sample == 0 && max == 10) printf("\e[1;31m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n", z_array[0], z_array[1], z_array[2], z_array[3], z_array[4], z_array[5], z_array[6], z_array[7], z_array[8], z_array[9]);

		start = utime();

		qsort(z_array, max, sizeof(int), cmp_int);

		end = utime();

		if (sample == 0 && max == 10) printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n", z_array[0], z_array[1], z_array[2], z_array[3], z_array[4], z_array[5], z_array[6], z_array[7], z_array[8], z_array[9]);

		total = end - start;

		if (!best || total < best)
		{
			best = total;
		}
	}
	printf("            qsort: sorted %d elements in %f seconds. (%s)\n", max, best / 1000000.0, desc);

	for (cnt = 1 ; cnt < max ; cnt++) if (z_array[cnt - 1] > z_array[cnt]) printf("            qsort: not properly sorted at index %d. (%d vs %d\n", cnt, z_array[cnt - 1], z_array[cnt]);
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
	size_t cnt, rnd;
	int *a_array, *b_array, *r_array;

	if (argc && argv[1] && *argv[1])
	{
		max = atoi(argv[1]);
	}

	rnd = 1;

	srand(rnd);

	a_array = malloc(max * sizeof(int));
	b_array = malloc(max * sizeof(int));
	r_array = malloc(max * sizeof(int));

	srand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	test_quad(a_array, r_array, max, "random order");

	test_quick(b_array, r_array, max, "random order");

	validate(a_array, b_array, max);


	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = cnt;
	}

	test_quad(a_array, r_array, max, "forward order");

	test_quick(b_array, r_array, max, "forward order");

	validate(a_array, b_array, max);


	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = max - cnt;
	}

	test_quad(a_array, r_array, max, "reverse order");

	test_quick(b_array, r_array, max, "reverse order");

	validate(a_array, b_array, max);


	srand(rnd);

	for (cnt = 0 ; cnt < max * 3 / 4 ; cnt++)
	{
		r_array[cnt] = cnt;
	}

	for (cnt = max * 3 / 4 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	test_quad(a_array, r_array, max, "random tail");

	test_quick(b_array, r_array, max, "random tail");

	validate(a_array, b_array, max);


	free(a_array);
	free(b_array);
	free(r_array);

	return 0;
}
