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
	To compile use:
	
	gcc -O3 bench.c
	
	or
	
	g++ -O3 -w -fpermissive bench.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <math.h>

#include "quadsort.h"

//#define cmp(a,b) (*(a) > *(b))

//typedef int CMPFUNC (const void *a, const void *b);

typedef void SRTFUNC(void *array, size_t nmemb, size_t size, CMPFUNC *cmpf);


// Must prevent inlining so the benchmark is fair against qsort.

// Remove __attribute__ ((noinline)) and comparisons++ for full throttle.

size_t comparisons;

__attribute__ ((noinline)) int cmp_int(const void * a, const void * b)
{
	int fa = *(int *)a;
	int fb = *(int *)b;

	comparisons++;

	return fa - fb;
}

__attribute__ ((noinline)) int cmp_rev(const void * a, const void * b)
{
	int fa = *(int *)a;
	int fb = *(int *)b;

	comparisons++;

	return fb - fa;
}

__attribute__ ((noinline)) int cmp_stable(const void * a, const void * b)
{
	int fa = *(int *)a;
	int fb = *(int *)b;

	comparisons++;

	return fa / 100000 - fb / 100000;
}

__attribute__ ((noinline)) int cmp_long(const void * a, const void * b)
{
	const long long fa = *(const long long *) a;
	const long long fb = *(const long long *) b;

	comparisons++;

	return (fa < fb ? -1 : fa > fb);
}

__attribute__ ((noinline)) int cmp_long_double(const void * a, const void * b)
{
	const long double fa = *(const long double *) a;
	const long double fb = *(const long double *) b;

	comparisons++;

	if (isnan(fa) || isnan(fb))
	{
		return isnan(fa) - isnan(fb);
	}
	return (fa < fb ? -1 : fa > fb);
}


int cmp_str(const void * a, const void * b)
{
	return strcmp(*(const char **) a, *(const char **) b);
}

int cmp_float(const void * a, const void * b)
{
	return *(float *) a - *(float *) b;
}


long long utime()
{
	struct timeval now_time;

	gettimeofday(&now_time, NULL);

	return now_time.tv_sec * 1000000LL + now_time.tv_usec;
}

void seed_rand(unsigned long long seed)
{
	srand(seed);
}

void test_sort(void *array, void *unsorted, void *valid, int minimum, int maximum, int samples, int repetitions, SRTFUNC *srt, const char *name, char *desc, size_t size, CMPFUNC *cmpf)
{
	long long start, end, total, best, average;
	size_t sam, max;
	long long *ptla = (long long *) array, *ptlv = valid;
	long double *ptda = (long double *) array, *ptdv = valid;
	int *pta = (int *) array, *ptv = (int *) valid, cnt;

	if (*name == '*')
	{
		if (!strcmp(desc, "random order") || !strcmp(desc, "random 1-4") || !strcmp(desc, "random 4"))
		{
			if (comparisons)
			{
				printf("%s\n", "|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |");
				printf("%s\n", "| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |");
			}
			else
			{
				printf("%s\n", "|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |");
				printf("%s\n", "| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |");
			}
		}
		else
		{
				printf("%s\n", "|           |          |      |          |          |           |         |                  |");
		}
		return;
	}

	best = average = 0;

	if (minimum == 7 && maximum == 7)
	{
		printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d\e[0m\n", pta[0], pta[1], pta[2], pta[3], pta[4], pta[5], pta[6]);
	}

	for (sam = 0 ; sam < samples ; sam++)
	{
		total = 0;

		max = minimum;

		if (repetitions > 1)
		{
			start = utime();

			for (int rep = repetitions - 1 ; rep >= 0 ; rep--)
			{
				memcpy(array, unsorted + maximum * rep, max * size);

				comparisons = 0;

				switch (*name)
				{
					case 'q':
						if (name[1] == 'u')
						{
							quadsort(array, max, size, cmpf);
						}
						else
						{
							qsort(array, max, size, cmpf);
						}
						break;
				}

				if (minimum < maximum && ++max > maximum)
				{
					max = minimum;
				}
			}
			end = utime();
		}
		else
		{
			memcpy(array, unsorted, max * size);

			comparisons = 0;

			start = utime();

			switch (*name)
			{
				case 'q':
					if (name[1] == 'u')
					{
						quadsort(array, max, size, cmpf);
					}
					else
					{
						qsort(array, max, size, cmpf);
					}
					break;
			}
			end = utime();
		}

		total = end - start;

		if (!best || total < best)
		{
			best = total;
		}
		average += total;
	}

	if (minimum == 7 && maximum == 7)
	{
		printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d\e[0m\n", pta[0], pta[1], pta[2], pta[3], pta[4], pta[5], pta[6]);
	}

	if (repetitions == 0)
	{
		return;
	}

	average /= samples;

	if (cmpf == cmp_stable)
	{
		for (cnt = 1 ; cnt < maximum ; cnt++)
		{
			if (pta[cnt - 1] > pta[cnt])
			{
				sprintf(desc, "\e[1;31m%16s\e[0m", "unstable");

				break;
			}
		}
	}

	if (comparisons)
	{
		printf("|%10s | %8d | %4d | %f | %f | %9d | %7d | %16s |\n", name, maximum, (int) size * 8, best / 1000000.0, average / 1000000.0, (int) comparisons, samples, desc);
	}
	else
	{
		printf("|%10s | %8d | %4d | %f | %f | %9d | %7d | %16s |\n", name, maximum, (int) size * 8, best / 1000000.0, average / 1000000.0, repetitions, samples, desc);
	}

	if (minimum != maximum || cmpf == cmp_stable)
	{
		return;
	}

	for (cnt = 1 ; cnt < maximum ; cnt++)
	{
		if (size == sizeof(int))
		{
			if (pta[cnt - 1] > pta[cnt])
			{
				printf("%17s: not properly sorted at index %d. (%d vs %d\n", name, cnt, pta[cnt - 1], pta[cnt]);
				break;
			}
			if (pta[cnt - 1] == pta[cnt])
			{
//				printf("%17s: Found a repeat value at index %d. (%d)\n", name, cnt, pta[cnt]);
			}
		}
		else if (size == sizeof(long long))
		{
			if (ptla[cnt - 1] > ptla[cnt])
			{
				printf("%17s: not properly sorted at index %d. (%lld vs %lld\n", name, cnt, ptla[cnt - 1], ptla[cnt]);
				break;
			}
		}
		else if (size == sizeof(long double))
		{
			if (cmp_long_double(&ptda[cnt - 1], &ptda[cnt]) > 0)
			{
				printf("%17s: not properly sorted at index %d. (%Lf vs %Lf\n", name, cnt, ptda[cnt - 1], ptda[cnt]);
				break;
			}
		}
	}

	for (cnt = 1 ; cnt < maximum ; cnt++)
	{
		if (size == sizeof(int))
		{
			if (pta[cnt] != ptv[cnt])
			{
				printf("         validate: array[%d] != valid[%d]. (%d vs %d\n", cnt, cnt, pta[cnt], ptv[cnt]);
				break;
			}
		}
		else if (size == sizeof(long long))
		{
			if (ptla[cnt] != ptlv[cnt])
			{
				printf("         validate: array[%d] != valid[%d]. (%lld vs %lld\n", cnt, cnt, ptla[cnt], ptlv[cnt]);
				break;
			}
		}
		else if (size == sizeof(long double))
		{
			if (ptda[cnt] != ptdv[cnt])
			{
				printf("         validate: array[%d] != valid[%d]. (%Lf vs %Lf\n", cnt, cnt, ptda[cnt], ptdv[cnt]);
				break;
			}
		}
	}
}

void validate()
{
	int seed = time(NULL);
	int cnt, val, max = 2000000;

	int *a_array, *r_array, *v_array;

	seed_rand(seed);

	a_array = (int *) malloc(max * sizeof(int));
	r_array = (int *) malloc(max * sizeof(int));
	v_array = (int *) malloc(max * sizeof(int));

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	for (cnt = 1 ; cnt < 100 ; cnt++)
	{
		memcpy(a_array, r_array, max * sizeof(int));
		memcpy(v_array, r_array, max * sizeof(int));

		quadsort(a_array, cnt, sizeof(int), cmp_int);
		qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val])
			{
				printf("\e[1;31mvalidate rand: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
				return;
			}

			if (a_array[val] != v_array[val])
			{
				printf("\e[1;31mvalidate rand: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
				return;
			}
		}
	}

	// ascending saw

	for (cnt = 0 ; cnt < 1000 ; cnt++)
	{
		r_array[cnt] = rand();
	}

	quadsort(r_array + max / 4 * 0, max / 4, sizeof(int), cmp_int);
	quadsort(r_array + max / 4 * 1, max / 4, sizeof(int), cmp_int);
	quadsort(r_array + max / 4 * 2, max / 4, sizeof(int), cmp_int);
	quadsort(r_array + max / 4 * 3, max / 4, sizeof(int), cmp_int);

	for (cnt = 1 ; cnt < 1000 ; cnt += 7)
	{
		memcpy(a_array, r_array, max * sizeof(int));
		memcpy(v_array, r_array, max * sizeof(int));

		quadsort(a_array, cnt, sizeof(int), cmp_int);
		qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val])
			{
				printf("\e[1;31mvalidate ascending saw: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
				return;
			}

			if (a_array[val] != v_array[val])
			{
				printf("\e[1;31mvalidate ascending saw: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
				return;
			}
		}
	}

	// descending saw

	for (cnt = 0 ; cnt < 1000 ; cnt++)
	{
		r_array[cnt] = (max - cnt - 1) % 100000;
	}

	for (cnt = 1 ; cnt < 1000 ; cnt += 7)
	{
		memcpy(a_array, r_array, max * sizeof(int));
		memcpy(v_array, r_array, max * sizeof(int));

		quadsort(a_array, cnt, sizeof(int), cmp_int);
		qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val])
			{
				printf("\e[1;31mvalidate descending saw: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
				return;
			}

			if (a_array[val] != v_array[val])
			{
				printf("\e[1;31mvalidate descending saw: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
				return;
			}
		}
	}

	// random tail

	for (cnt = 0 ; cnt < max * 3 / 4 ; cnt++)
	{
		r_array[cnt] = cnt;
	}

	for (cnt = max * 3 / 4 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	for (cnt = 1 ; cnt < 1000 ; cnt += 7)
	{
		memcpy(a_array, r_array, max * sizeof(int));
		memcpy(v_array, r_array, max * sizeof(int));

		quadsort(a_array, cnt, sizeof(int), cmp_int);
		qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val])
			{
				printf("\e[1;31mvalidate rand tail: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
				return;
			}

			if (a_array[val] != v_array[val])
			{
				printf("\e[1;31mvalidate rand tail: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
				return;
			}
		}
	}

	free(a_array);
	free(r_array);
	free(v_array);
}


int main(int argc, char **argv)
{
	int max = 100000;
	int samples = 10;
	int repetitions = 1;
	int seed = 0;
	int cnt, rnd, lst, rep;
	size_t mem;
	int *a_array, *r_array, *v_array;
	long long *la_array, *lr_array, *lv_array;
	long double *da_array, *dr_array, *dv_array;

	char dist[40], *sorts[] = { "*", "qsort", "quadsort" };

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

	if (argc >= 4 && argv[4] && *argv[4])
	{
		seed = atoi(argv[4]);
	}

	validate();

	rnd = seed ? seed : time(NULL);

	printf("Info: int = %lu, long long = %lu, long double = %lu\n\n", sizeof(int) * 8, sizeof(long long) * 8, sizeof(long double) * 8);

	printf("Benchmark: array size: %d, samples: %d, repetitions: %d, seed: %d\n\n", max, samples, repetitions, rnd);

	if (samples == 0 && repetitions == 0)
	{
		goto small_range_test;
	}

	mem = max * repetitions;

	// 128 bit

	da_array = (long double *) malloc(max * sizeof(long double));
	dr_array = (long double *) malloc(mem * sizeof(long double));
	dv_array = (long double *) malloc(max * sizeof(long double));

	if (da_array == NULL || dr_array == NULL || dv_array == NULL)
	{
		printf("main(%d,%d,%d): malloc: %s\n", max, samples, repetitions, strerror(errno));

		return 0;
	}

	seed_rand(rnd);

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		dr_array[cnt] = rand() + 1.0 / (long double) (rand() + (rand() << 30LL));
	}

	memcpy(dv_array, dr_array, max * sizeof(long double));
	quadsort(dv_array, max, sizeof(long double), cmp_long_double);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(da_array, dr_array, dv_array, max, max, samples, repetitions, qsort, sorts[cnt], "random order", sizeof(long double), cmp_long_double);
	}

	free(da_array);
	free(dr_array);
	free(dv_array);

	printf("\n");
	
	// 64 bit

	la_array = (long long *) malloc(max * sizeof(long long));
	lr_array = (long long *) malloc(mem * sizeof(long long));
	lv_array = (long long *) malloc(max * sizeof(long long));

	if (la_array == NULL || lr_array == NULL || lv_array == NULL)
	{
		printf("main(%d,%d,%d): malloc: %s\n", max, samples, repetitions, strerror(errno));

		return 0;
	}

	seed_rand(rnd);

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		lr_array[cnt] = rand();
		lr_array[cnt] += (unsigned long long) rand() << 32ULL;
	}

	memcpy(lv_array, lr_array, max * sizeof(long long));
	quadsort(lv_array, max, sizeof(long long), cmp_long);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(la_array, lr_array, lv_array, max, max, samples, repetitions, qsort, sorts[cnt], "random order", sizeof(long long), cmp_long);
	}

	printf("\n");

	free(la_array);
	free(lr_array);
	free(lv_array);

	// 32 bit

	a_array = (int *) malloc(max * sizeof(int));
	r_array = (int *) malloc(mem * sizeof(int));
	v_array = (int *) malloc(max * sizeof(int));

	// random

	seed_rand(rnd);

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = rand();
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "random order", sizeof(int), cmp_int);
	}

	// ascending

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = cnt;
	}

	memcpy(v_array, r_array, max * sizeof(int));
	memcpy(r_array, v_array, max * sizeof(int));

	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "ascending order", sizeof(int), cmp_int);
	}

	// ascending saw

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array + max / 4 * 0, max / 4, sizeof(int), cmp_int);
	quadsort(v_array + max / 4 * 1, max / 4, sizeof(int), cmp_int);
	quadsort(v_array + max / 4 * 2, max / 4, sizeof(int), cmp_int);
	quadsort(v_array + max / 4 * 3, max / 4, sizeof(int), cmp_int);

	for (rep = 0 ; rep < repetitions ; rep++)
	{
		memcpy(r_array + rep * max, v_array, max * sizeof(int));
	}

	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "ascending saw", sizeof(int), cmp_int);
	}

	// generic

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = rand() % 100;
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "generic order", sizeof(int), cmp_int);
	}

	// descending

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = (mem - cnt);
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "descending order", sizeof(int), cmp_int);
	}

	// descending saw

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array + max / 4 * 0, max / 4, sizeof(int), cmp_rev);
	quadsort(v_array + max / 4 * 1, max / 4, sizeof(int), cmp_rev);
	quadsort(v_array + max / 4 * 2, max / 4, sizeof(int), cmp_rev);
	quadsort(v_array + max / 4 * 3, max / 4, sizeof(int), cmp_rev);

	for (rep = 0 ; rep < repetitions ; rep++)
	{
		memcpy(r_array + rep * max, v_array, max * sizeof(int));
	}

	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "descending saw", sizeof(int), cmp_int);
	}

	// random tail

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max * 3 / 4, sizeof(int), cmp_int);

	memcpy(r_array, v_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "random tail", sizeof(int), cmp_int);
	}

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max / 2, sizeof(int), cmp_int);

	for (rep = 0 ; rep < repetitions ; rep++)
	{
		memcpy(r_array + rep * max, v_array, max * sizeof(int));
	}

	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "random half", sizeof(int), cmp_int);
	}

	// tiles

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		if (cnt % 2 == 0)
		{
			r_array[cnt] = 16777216 + cnt;
		}
		else
		{
			r_array[cnt] = 33554432 + cnt;
		}
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		strcpy(dist, "ascending tiles");

		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], dist, sizeof(int), cmp_stable);
	}

	if (repetitions > 0)
	{
		goto end;
	}

	small_range_test:

	mem = 32768 * 32;

	a_array = (int *) malloc(max * sizeof(int));
	r_array = (int *) malloc(mem * sizeof(int));
	v_array = (int *) malloc(max * sizeof(int));

	srand(rnd);

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = rand();
	}

	if (max >= 8192)
	{
		goto large_range_test;
	}

	for (lst = 1, samples = 32768*8, repetitions = 4 ; repetitions <= samples && repetitions <= 4096 ; repetitions *= 2, samples /= 2)
	{
		if (max >= repetitions)
		{
			sprintf(dist, "random %d-%d", lst, repetitions);

			memcpy(v_array, r_array, repetitions * sizeof(int));
			quadsort(v_array, repetitions, sizeof(int), cmp_int);

			for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
			{
				test_sort(a_array, r_array, v_array, lst, repetitions, 100, samples, qsort, sorts[cnt], dist, sizeof(int), cmp_int);
			}
			lst = repetitions + 1;
		}
	}

	goto end;

	large_range_test:

	for (samples = 32768, repetitions = 4 ; samples > 0 ; repetitions *= 2, samples /= 2)
	{
		if (max >= repetitions)
		{
			memcpy(v_array, r_array, repetitions * sizeof(int));
			quadsort(v_array, repetitions, sizeof(int), cmp_int);

			sprintf(dist, "random %d", repetitions);

			for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
			{
				test_sort(a_array, r_array, v_array, repetitions, repetitions, 100, samples, qsort, sorts[cnt], dist, sizeof(int), cmp_int);
			}
		}
	}

	end:


	free(a_array);
	free(r_array);
	free(v_array);

	return 0;
}
