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
	quadsort 1.1.1.1
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>

typedef int CMPFUNC (const void *a, const void *b);

void quad_swap32(void *array, void *swap, size_t nmemb, CMPFUNC *cmp)
{
	size_t offset;
	register int *pts, *pta;

	pta = array;
	pts = swap;

	for (offset = 0 ; offset + 4 <= nmemb ; offset += 4)
	{
		if (cmp(&pta[0], &pta[1]) > 0)
		{
			pts[0] = pta[1];
			pts[1] = pta[0];
		}
		else
		{
			pts[0] = pta[0];
			pts[1] = pta[1];
		}

		if (cmp(&pta[2], &pta[3]) > 0)
		{
			pts[2] = pta[3];
			pts[3] = pta[2];
		}
		else
		{
			pts[2] = pta[2];
			pts[3] = pta[3];
		}

		if (cmp(&pts[1], &pts[2]) <= 0)
		{
			*pta++ = pts[0];
			*pta++ = pts[1];
			*pta++ = pts[2];
			*pta++ = pts[3];
		}
		else if (cmp(&pts[0], &pts[3]) > 0)
		{
			*pta++ = pts[2];
			*pta++ = pts[3];
			*pta++ = pts[0];
			*pta++ = pts[1];
		}
		else if (cmp(&pts[0], &pts[2]) > 0)
		{
			*pta++ = pts[2];
			*pta++ = pts[0];

			if (cmp(&pts[1], &pts[3]) > 0)
			{
				*pta++ = pts[3];
				*pta++ = pts[1];
			}
			else
			{
				*pta++ = pts[1];
				*pta++ = pts[3];
			}
		}
		else
		{
			*pta++ = pts[0];
			*pta++ = pts[2];

			if (cmp(&pts[1], &pts[3]) > 0)
			{
				*pta++ = pts[3];
				*pta++ = pts[1];
			}
			else
			{
				*pta++ = pts[1];
				*pta++ = pts[3];
			}
		}
	}

	switch (nmemb - offset)
	{
		case 0:
		case 1:
			return;
		case 2:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts[0] = pta[0];
				pta[0] = pta[1];
				pta[1] = pts[0];
			}
			return;
		case 3:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts[0] = pta[0];
				pta[0] = pta[1];
				pta[1] = pts[0];
			}
			if (cmp(&pta[1], &pta[2]) > 0)
			{
				pts[0] = pta[1];
				pta[1] = pta[2];
				pta[2] = pts[0];
			}
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts[0] = pta[0];
				pta[0] = pta[1];
				pta[1] = pts[0];
			}
			return;
		default:
			assert(nmemb < 1 && nmemb > 3);
	}
}

void quad_swap64(void *array, void *swap, size_t nmemb, CMPFUNC *cmp)
{
	size_t offset;
	register long long *pts, *pta;

	pta = array;
	pts = swap;

	for (offset = 0 ; offset + 4 <= nmemb ; offset += 4)
	{
		if (cmp(&pta[0], &pta[1]) > 0)
		{
			pts[0] = pta[1];
			pts[1] = pta[0];
		}
		else
		{
			pts[0] = pta[0];
			pts[1] = pta[1];
		}

		if (cmp(&pta[2], &pta[3]) > 0)
		{
			pts[2] = pta[3];
			pts[3] = pta[2];
		}
		else
		{
			pts[2] = pta[2];
			pts[3] = pta[3];
		}

		if (cmp(&pts[1], &pts[2]) <= 0)
		{
			*pta++ = pts[0];
			*pta++ = pts[1];
			*pta++ = pts[2];
			*pta++ = pts[3];
		}
		else if (cmp(&pts[0], &pts[3]) > 0)
		{
			*pta++ = pts[2];
			*pta++ = pts[3];
			*pta++ = pts[0];
			*pta++ = pts[1];
		}
		else if (cmp(&pts[0], &pts[2]) > 0)
		{
			*pta++ = pts[2];
			*pta++ = pts[0];

			if (cmp(&pts[1], &pts[3]) > 0)
			{
				*pta++ = pts[3];
				*pta++ = pts[1];
			}
			else
			{
				*pta++ = pts[1];
				*pta++ = pts[3];
			}
		}
		else
		{
			*pta++ = pts[0];
			*pta++ = pts[2];

			if (cmp(&pts[1], &pts[3]) > 0)
			{
				*pta++ = pts[3];
				*pta++ = pts[1];
			}
			else
			{
				*pta++ = pts[1];
				*pta++ = pts[3];
			}
		}
	}

	switch (nmemb - offset)
	{
		case 0:
		case 1:
			return;
		case 2:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts[0] = pta[0];
				pta[0] = pta[1];
				pta[1] = pts[0];
			}
			return;
		case 3:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts[0] = pta[0];
				pta[0] = pta[1];
				pta[1] = pts[0];
			}
			if (cmp(&pta[1], &pta[2]) > 0)
			{
				pts[0] = pta[1];
				pta[1] = pta[2];
				pta[2] = pts[0];
			}
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts[0] = pta[0];
				pta[0] = pta[1];
				pta[1] = pts[0];
			}
			return;
		default:
			assert(nmemb < 1 && nmemb > 3);
	}
}

void quad_sort32(void *array, void *swap, size_t nmemb, CMPFUNC *cmp)
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

						while (c < c_max)
							*pts++ = *c++;

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
				while (d < d_max)
					*pts++ = *d++;

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
				{
					*pts++ = *c++;
				}
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
						while (d < d_max)
							*pts++ = *d++;
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
					while (c < end)
						*pts++ = *c++;
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
					while (d < d_max)
						*pta++ = *d++;
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

						while (c < c_max)
							*pts++ = *c++;

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
				while (d < d_max)
					*pts++ = *d++;

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
				{
					*pts++ = *c++;
				}
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
						while (d < d_max)
							*pts++ = *d++;
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
					while (c < end)
						*pts++ = *c++;
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
					while (d < d_max)
						*pta++ = *d++;
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

		quad_sort32(array, swap, nmemb, cmp);
	}
	else if (size == sizeof(long long))
	{
		quad_swap64(array, swap, nmemb, cmp);

		quad_sort64(array, swap, nmemb, cmp);
	}
	else
	{
		assert(size == 4 || size == 8);
	}

	free(swap);
}

int cmp_int(const void * a, const void * b)
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
	long long start, end, cnt;

	memcpy(z_array, r_array, max * sizeof(int));

	if (max <= 10) printf("\e[1;31m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\n", z_array[0], z_array[1], z_array[2], z_array[3], z_array[4], z_array[5], z_array[6], z_array[7], z_array[8], z_array[9]);

	start = utime();

	quadsort(z_array, max, sizeof(int), cmp_int);

	end = utime();

	if (max <= 10) printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\n", z_array[0], z_array[1], z_array[2], z_array[3], z_array[4], z_array[5], z_array[6], z_array[7], z_array[8], z_array[9]);

	printf("\e[0m        Quad Sort: sorted %d elements in %f seconds. (%s)\n", max, (end - start) / 1000000.0, desc);

	for (cnt = 1 ; cnt < max ; cnt++) if (z_array[cnt - 1] > z_array[cnt]) {printf("        Quad Sort: not properly sorted at index %lld. (%d vs %d\n", cnt, z_array[cnt - 1], z_array[cnt]);break;}
}

void test_quick(int *z_array, int *r_array, int max, char *desc)
{
	long long start, end, cnt;

	memcpy(z_array, r_array, max * sizeof(int));

	if (max <= 10) printf("\e[1;31m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\n", z_array[0], z_array[1], z_array[2], z_array[3], z_array[4], z_array[5], z_array[6], z_array[7], z_array[8], z_array[9]);

	start = utime();

	qsort(z_array, max, sizeof(int), cmp_int);

	end = utime();

	if (max <= 10) printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\n", z_array[0], z_array[1], z_array[2], z_array[3], z_array[4], z_array[5], z_array[6], z_array[7], z_array[8], z_array[9]);

	printf("\e[0m       Quick Sort: sorted %d elements in %f seconds. (%s)\n", max, (end - start) / 1000000.0, desc);

	for (cnt = 1 ; cnt < max ; cnt++) if (z_array[cnt - 1] > z_array[cnt]) {printf("       Quick Sort: not properly sorted at index %lld. (%d vs %d\n", cnt, z_array[cnt - 1], z_array[cnt]);break;}
}

int main(int argc, char **argv)
{
	static int max = 1000000;
	int cnt, rnd;
	int *z_array, *r_array;

	rnd = 1;

	srand(rnd);

	z_array = malloc(max * sizeof(int));
	r_array = malloc(max * sizeof(int));

	srand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	test_quad(z_array, r_array, max, "random order");

	test_quick(z_array, r_array, max, "random order");

	printf("\n");

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = cnt;
	}

	test_quad(z_array, r_array, max, "forward order");

	test_quick(z_array, r_array, max, "forward order");

	printf("\n");

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = max - cnt;
	}

	test_quad(z_array, r_array, max, "reverse order");

	test_quick(z_array, r_array, max, "reverse order");

	printf("\n");

	srand(rnd);

	for (cnt = 0 ; cnt < max * 3 / 4 ; cnt++)
	{
		r_array[cnt] = cnt;
	}

	for (cnt = max * 3 / 4 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	test_quad(z_array, r_array, max, "random tail");

	test_quick(z_array, r_array, max, "random tail");

	printf("\n");

	free(z_array);
	free(r_array);

	return 0;
}
