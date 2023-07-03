/*
	Copyright (C) 2014-2023 Igor van den Hoven ivdhoven@gmail.com
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
	To compile use either:

	gcc -O3 bench.c

	or

	clang -O3 bench.c

	or

	g++ -O3 bench.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <math.h>

//#define cmp(a,b) (*(a) > *(b)) // uncomment for faster primitive comparisons

const char *sorts[] = { "*", "qsort", "quadsort" };

//#define SKIP_STRINGS
//#define SKIP_DOUBLES
//#define SKIP_LONGS

#if __has_include("blitsort.h")
  #include "blitsort.h" // curl "https://raw.githubusercontent.com/scandum/blitsort/master/src/blitsort.{c,h}" -o "blitsort.#1"
#endif
#if __has_include("crumsort.h")
  #include "crumsort.h" // curl "https://raw.githubusercontent.com/scandum/crumsort/master/src/crumsort.{c,h}" -o "crumsort.#1"
#endif
#if __has_include("dripsort.h")
  #include "dripsort.h"
#endif
#if __has_include("flowsort.h")
  #include "flowsort.h"
#endif
#if __has_include("fluxsort.h")
  #include "fluxsort.h" // curl "https://raw.githubusercontent.com/scandum/fluxsort/master/src/fluxsort.{c,h}" -o "fluxsort.#1"
#endif
#if __has_include("gridsort.h")
  #include "gridsort.h" // curl "https://raw.githubusercontent.com/scandum/gridsort/master/src/gridsort.{c,h}" -o "gridsort.#1"
#endif
#if __has_include("octosort.h")
  #include "octosort.h" // curl "https://raw.githubusercontent.com/scandum/octosort/master/src/octosort.{c,h}" -o "octosort.#1"
#endif
#if __has_include("piposort.h")
  #include "piposort.h" // curl "https://raw.githubusercontent.com/scandum/piposort/master/src/piposort.{c,h}" -o "piposort.#1"
#endif
#if __has_include("quadsort.h")
  #include "quadsort.h" // curl "https://raw.githubusercontent.com/scandum/quadsort/master/src/quadsort.{c,h}" -o "quadsort.#1"
#endif
#if __has_include("skipsort.h")
  #include "skipsort.h"
#endif
#if __has_include("wolfsort.h")
  #include "wolfsort.h" // curl "https://raw.githubusercontent.com/scandum/wolfsort/master/src/wolfsort.{c,h}" -o "wolfsort.#1"
#endif

#if __has_include("rhsort.c")
    #define RHSORT_C
    #include "rhsort.c" // curl https://raw.githubusercontent.com/mlochbaum/rhsort/master/rhsort.c > rhsort.c
#endif

#ifdef __GNUG__
  #include <algorithm>
  #if __has_include("pdqsort.h")
    #include "pdqsort.h" // curl https://raw.githubusercontent.com/orlp/pdqsort/master/pdqsort.h > pdqsort.h
  #endif
  #if __has_include("ska_sort.hpp")
    #define SKASORT_HPP
    #include "ska_sort.hpp" // curl https://raw.githubusercontent.com/skarupke/ska_sort/master/ska_sort.hpp > ska_sort.hpp
  #endif
  #if __has_include("timsort.hpp")
    #include "timsort.hpp" // curl https://raw.githubusercontent.com/timsort/cpp-TimSort/master/include/gfx/timsort.hpp > timsort.hpp
  #endif
#endif

#if __has_include("antiqsort.c")
  #include "antiqsort.c"
#endif

//typedef int CMPFUNC (const void *a, const void *b);

typedef void SRTFUNC(void *array, size_t nmemb, size_t size, CMPFUNC *cmpf);


// Comment out Remove __attribute__ ((noinline)) and comparisons++ for full
// throttle. Like so: #define COMPARISON_PP //comparisons++ 

size_t comparisons;

#define COMPARISON_PP comparisons++

#define NO_INLINE __attribute__ ((noinline))

// primitive type comparison functions

NO_INLINE int cmp_int(const void * a, const void * b)
{
	COMPARISON_PP;

	return *(int *) a - *(int *) b;

//	const int l = *(const int *)a;
//	const int r = *(const int *)b;

//	return l - r;
//	return l > r;
//	return (l > r) - (l < r);
}

NO_INLINE int cmp_rev(const void * a, const void * b)
{
	int fa = *(int *)a;
	int fb = *(int *)b;

	COMPARISON_PP;

	return fb - fa;
}

NO_INLINE int cmp_stable(const void * a, const void * b)
{
	int fa = *(int *)a;
	int fb = *(int *)b;

	COMPARISON_PP;

	return fa / 100000 - fb / 100000;
}

NO_INLINE int cmp_long(const void * a, const void * b)
{
	const long long fa = *(const long long *) a;
	const long long fb = *(const long long *) b;

	COMPARISON_PP;

	return (fa > fb) - (fa < fb);
//	return (fa > fb);
}

NO_INLINE int cmp_float(const void * a, const void * b)
{
	return *(float *) a - *(float *) b;
}

NO_INLINE int cmp_long_double(const void * a, const void * b)
{
	const long double fa = *(const long double *) a;
	const long double fb = *(const long double *) b;

	COMPARISON_PP;

	return (fa > fb) - (fa < fb);

/*	if (isnan(fa) || isnan(fb))
	{
		return isnan(fa) - isnan(fb);
	}

	return (fa > fb);
*/
}

// pointer comparison functions

NO_INLINE int cmp_str(const void * a, const void * b)
{
	COMPARISON_PP;

	return strcmp(*(const char **) a, *(const char **) b);
}

NO_INLINE int cmp_int_ptr(const void * a, const void * b)
{
	const int *fa = *(const int **) a;
	const int *fb = *(const int **) b;

	COMPARISON_PP;

	return (*fa > *fb) - (*fa < *fb);
}

NO_INLINE int cmp_long_ptr(const void * a, const void * b)
{
	const long long *fa = *(const long long **) a;
	const long long *fb = *(const long long **) b;

	COMPARISON_PP;

	return (*fa > *fb) - (*fa < *fb);
}

NO_INLINE int cmp_long_double_ptr(const void * a, const void * b)
{
	const long double *fa = *(const long double **) a;
	const long double *fb = *(const long double **) b;

	COMPARISON_PP;

	return (*fa > *fb) - (*fa < *fb);
}

// c++ comparison functions

#ifdef __GNUG__

NO_INLINE bool cpp_cmp_int(const int &a, const int &b)
{
	COMPARISON_PP;

	return a < b;
}

NO_INLINE bool cpp_cmp_str(char const* const a, char const* const b)
{
	COMPARISON_PP;

	return strcmp(a, b) < 0;
}

#endif

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

void test_sort(void *array, void *unsorted, void *valid, int minimum, int maximum, int samples, int repetitions, SRTFUNC *srt, const char *name, const char *desc, size_t size, CMPFUNC *cmpf)
{
	long long start, end, total, best, average_time, average_comp;
	char temp[100];
	static char compare = 0;
	long long *ptla = (long long *) array, *ptlv = (long long *) valid;
	long double *ptda = (long double *) array, *ptdv = (long double *) valid;
	int *pta = (int *) array, *ptv = (int *) valid, rep, sam, max, cnt, name32;

#ifdef SKASORT_HPP
	void *swap;
#endif

	if (*name == '*')
	{
		if (!strcmp(desc, "random order") || !strcmp(desc, "random 1-4") || !strcmp(desc, "random 4") || !strcmp(desc, "random string") || !strcmp(desc, "random 10"))
		{
			if (comparisons)
			{
				compare = 1;
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

	name32 = name[0] + (name[1] ? name[1] * 32 : 0) + (name[2] ? name[2] * 1024 : 0);

	best = average_time = average_comp = 0;

	if (minimum == 7 && maximum == 7)
	{
		printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d\e[0m\n", pta[0], pta[1], pta[2], pta[3], pta[4], pta[5], pta[6]);
	}

	for (sam = 0 ; sam < samples ; sam++)
	{
		total = average_comp = 0;
		max = minimum;

		start = utime();

		for (rep = repetitions - 1 ; rep >= 0 ; rep--)
		{
			memcpy(array, (char *) unsorted + maximum * rep * size, max * size);

			comparisons = 0;

			// edit char *sorts to add / remove sorts

			switch (name32)
			{
#ifdef BLITSORT_H
				case 'b' + 'l' * 32 + 'i' * 1024: blitsort(array, max, size, cmpf); break;
#endif
#ifdef CRUMSORT_H
				case 'c' + 'r' * 32 + 'u' * 1024: crumsort(array, max, size, cmpf); break;
#endif
#ifdef DRIPSORT_H
				case 'd' + 'r' * 32 + 'i' * 1024: dripsort(array, max, size, cmpf); break;
#endif
#ifdef FLOWSORT_H
				case 'f' + 'l' * 32 + 'o' * 1024: flowsort(array, max, size, cmpf); break;
#endif
#ifdef FLUXSORT_H
				case 'f' + 'l' * 32 + 'u' * 1024: fluxsort(array, max, size, cmpf); break;
				case 's' + '_' * 32 + 'f' * 1024: fluxsort_size(array, max, size, cmpf); break;

#endif
#ifdef GRIDSORT_H
				case 'g' + 'r' * 32 + 'i' * 1024: gridsort(array, max, size, cmpf); break;
#endif
#ifdef OCTOSORT_H
				case 'o' + 'c' * 32 + 't' * 1024: octosort(array, max, size, cmpf); break;
#endif
#ifdef PIPOSORT_H
				case 'p' + 'i' * 32 + 'p' * 1024: piposort(array, max, size, cmpf); break;
#endif
#ifdef QUADSORT_H
				case 'q' + 'u' * 32 + 'a' * 1024: quadsort(array, max, size, cmpf); break;
				case 's' + '_' * 32 + 'q' * 1024: quadsort_size(array, max, size, cmpf); break;
#endif
#ifdef SKIPSORT_H
				case 's' + 'k' * 32 + 'i' * 1024: skipsort(array, max, size, cmpf); break;
#endif
#ifdef WOLFSORT_H
				case 'w' + 'o' * 32 + 'l' * 1024: wolfsort(array, max, size, cmpf); break;
#endif
				case 'q' + 's' * 32 + 'o' * 1024: qsort(array, max, size, cmpf); break;

#ifdef RHSORT_C
				case 'r' + 'h' * 32 + 's' * 1024: if (size == sizeof(int)) rhsort32(pta, max); else return; break;
#endif

#ifdef __GNUG__
				case 's' + 'o' * 32 + 'r' * 1024: if (size == sizeof(int)) std::sort(pta, pta + max); else if (size == sizeof(long long)) std::sort(ptla, ptla + max); else std::sort(ptda, ptda + max); break;
				case 's' + 't' * 32 + 'a' * 1024: if (size == sizeof(int)) std::stable_sort(pta, pta + max); else if (size == sizeof(long long)) std::stable_sort(ptla, ptla + max); else std::stable_sort(ptda, ptda + max); break;

  #ifdef PDQSORT_H
				case 'p' + 'd' * 32 + 'q' * 1024: if (size == sizeof(int)) pdqsort(pta, pta + max); else if (size == sizeof(long long)) pdqsort(ptla, ptla + max); else pdqsort(ptda, ptda + max); break;
  #endif
  #ifdef SKASORT_HPP
				case 's' + 'k' * 32 + 'a' * 1024: swap = malloc(max * size); if (size == sizeof(int)) ska_sort_copy(pta, pta + max, (int *) swap); else if (size == sizeof(long long)) ska_sort_copy(ptla, ptla + max, (long long *) swap); else repetitions = 0; free(swap); break;
  #endif
  #ifdef GFX_TIMSORT_HPP
				case 't' + 'i' * 32 + 'm' * 1024: if (size == sizeof(int)) gfx::timsort(pta, pta + max, cpp_cmp_int); else if (size == sizeof(long long)) gfx::timsort(ptla, ptla + max); else gfx::timsort(ptda, ptda + max); break;
  #endif
#endif
				default:
					switch (name32)
					{
						case 's' + 'o' * 32 + 'r' * 1024:
						case 's' + 't' * 32 + 'a' * 1024:
						case 'p' + 'd' * 32 + 'q' * 1024: 
						case 'r' + 'h' * 32 + 's' * 1024:
						case 's' + 'k' * 32 + 'a' * 1024:
						case 't' + 'i' * 32 + 'm' * 1024:
							printf("unknown sort: %s (compile with g++ instead of gcc?)\n", name);
							return;
						default:
							printf("unknown sort: %s\n", name);
							return;
					}
			}
			average_comp += comparisons;

			if (minimum < maximum && ++max > maximum)
			{
				max = minimum;
			}
		}
		end = utime();

		total = end - start;

		if (!best || total < best)
		{
			best = total;
		}
		average_time += total;
	}

	if (minimum == 7 && maximum == 7)
	{
		printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d\e[0m\n", pta[0], pta[1], pta[2], pta[3], pta[4], pta[5], pta[6]);
	}

	if (repetitions == 0)
	{
		return;
	}

	average_time /= samples;

	if (cmpf == cmp_stable)
	{
		for (cnt = 1 ; cnt < maximum ; cnt++)
		{
			if (pta[cnt - 1] > pta[cnt])
			{
				sprintf(temp, "\e[1;31m%16s\e[0m", "unstable");
				desc = temp;
				break;
			}
		}
	}

	if (compare)
	{
		if (repetitions <= 1)
		{
			printf("|%10s |%9d | %4d |%9f |%9f |%10d | %7d | %16s |\e[0m\n", name, maximum, (int) size * 8, best / 1000000.0, average_time / 1000000.0, (int) comparisons, samples, desc);
		}
		else
		{
			printf("|%10s |%9d | %4d |%9f |%9f |%10.1f | %7d | %16s |\e[0m\n", name, maximum, (int) size * 8, best / 1000000.0, average_time / 1000000.0, (float) average_comp / repetitions, samples, desc);
		}
	}
	else
	{
		printf("|%10s | %8d | %4d | %f | %f | %9d | %7d | %16s |\e[0m\n", name, maximum, (int) size * 8, best / 1000000.0, average_time / 1000000.0, repetitions, samples, desc);
	}

	if (minimum != maximum || cmpf == cmp_stable)
	{
		return;
	}

	for (cnt = 1 ; cnt < maximum ; cnt++)
	{
		if (cmpf == cmp_str)
		{
			char **ptsa = (char **) array;
			if (strcmp((char *) ptsa[cnt - 1], (char *) ptsa[cnt]) > 0)
			{
				printf("%17s: not properly sorted at index %d. (%s vs %s\n", name, cnt, (char *) ptsa[cnt - 1], (char *) ptsa[cnt]);
				break;
			}
		}
		else if (size == sizeof(int *) && cmpf == cmp_long_double_ptr)
		{
			long double **pptda = (long double **) array;

			if (cmp_long_double_ptr(&pptda[cnt - 1], &pptda[cnt]) > 0)
			{
				printf("%17s: not properly sorted at index %d. (%Lf vs %Lf\n", name, cnt, *pptda[cnt - 1], *pptda[cnt]);
				break;
			}
		}
		else if (cmpf == cmp_long_ptr)
		{
			long long **pptla = (long long **) array;

			if (cmp_long_ptr(&pptla[cnt - 1], &pptla[cnt]) > 0)
			{
				printf("%17s: not properly sorted at index %d. (%lld vs %lld\n", name, cnt, *pptla[cnt - 1], *pptla[cnt]);
				break;
			}
		}
		else if (cmpf == cmp_int_ptr)
		{
			int **pptia = (int **) array;

			if (cmp_int_ptr(&pptia[cnt - 1], &pptia[cnt]) > 0)
			{
				printf("%17s: not properly sorted at index %d. (%d vs %d\n", name, cnt, *pptia[cnt - 1], *pptia[cnt]);
				break;
			}
		}
		else if (size == sizeof(int))
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
				if (cmpf == cmp_str)
				{
					char **ptsa = (char **) array;
					char **ptsv = (char **) valid;

					printf("         validate: array[%d] != valid[%d]. (%s vs %s) %s\n", cnt, cnt, (char *) ptsa[cnt], (char *) ptsv[cnt], !strcmp((char *) ptsa[cnt], (char *) ptsv[cnt]) ? "\e[1;31munstable\e[0m" : "");
					break;
				}
				if (cmpf == cmp_long_ptr)
				{
					long long **ptla = (long long **) array;
					long long **ptlv = (long long **) valid;

					printf("         validate: array[%d] != valid[%d]. (%lld vs %lld) %s\n", cnt, cnt, *ptla[cnt], *ptlv[cnt], (*ptla[cnt] == *ptlv[cnt]) ? "\e[1;31munstable\e[0m" : "");
					break;
				}
				if (cmpf == cmp_int_ptr)
				{
					int **ptia = (int **) array;
					int **ptiv = (int **) valid;

					printf("         validate: array[%d] != valid[%d]. (%d vs %d) %s\n", cnt, cnt, *ptia[cnt], *ptiv[cnt], (*ptia[cnt] == *ptiv[cnt]) ? "\e[1;31munstable\e[0m" : "");
					break;
				}

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
	int cnt, val, max = 1000;

	int *a_array, *r_array, *v_array;

	seed_rand(seed);

	a_array = (int *) malloc(max * sizeof(int));
	r_array = (int *) malloc(max * sizeof(int));
	v_array = (int *) malloc(max * sizeof(int));

	for (cnt = 0 ; cnt < max ; cnt++) r_array[cnt] = rand();

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		memcpy(a_array, r_array, cnt * sizeof(int));
		memcpy(v_array, r_array, cnt * sizeof(int));

		quadsort_prim(a_array, cnt, sizeof(int));
		qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val]) {printf("\e[1;31mvalidate rand: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val); return;}
			if (a_array[val] != v_array[val])           {printf("\e[1;31mvalidate rand: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val); return;}
		}
	}

	// ascending saw

	for (cnt = 0 ; cnt < max ; cnt++) r_array[cnt] = cnt % (max / 5);

	for (cnt = 0 ; cnt < max ; cnt += 7)
	{
		memcpy(a_array, r_array, cnt * sizeof(int));
		memcpy(v_array, r_array, cnt * sizeof(int));

		quadsort(a_array, cnt, sizeof(int), cmp_int);
		qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val]) {printf("\e[1;31mvalidate ascending saw: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val); return;}
			if (a_array[val] != v_array[val])           {printf("\e[1;31mvalidate ascending saw: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val); return;}
		}
	}

	// descending saw

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = (max - cnt + 1) % (max / 11);
	}

	for (cnt = 1 ; cnt < max ; cnt += 7)
	{
		memcpy(a_array, r_array, cnt * sizeof(int));
		memcpy(v_array, r_array, cnt * sizeof(int));

		quadsort(a_array, cnt, sizeof(int), cmp_int);
		qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val]) {printf("\e[1;31mvalidate descending saw: seed %d: size: %d Not properly sorted at index %d.\n\n", seed, cnt, val); return;}
			if (a_array[val] != v_array[val])           {printf("\e[1;31mvalidate descending saw: seed %d: size: %d Not verified at index %d.\n\n", seed, cnt, val); return;}
		}
	}

	// random half

	for (cnt = 0 ; cnt < max ; cnt++) r_array[cnt] = (cnt < max / 2) ? cnt : rand();

	for (cnt = 1 ; cnt < max ; cnt += 7)
	{
		memcpy(a_array, r_array, cnt * sizeof(int));
		memcpy(v_array, r_array, cnt * sizeof(int));

		quadsort(a_array, cnt, sizeof(int), cmp_int);
		qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val]) {printf("\e[1;31mvalidate rand tail: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val); return;}
			if (a_array[val] != v_array[val])           {printf("\e[1;31mvalidate rand tail: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val); return;}
		}
	}
	free(a_array);
	free(r_array);
	free(v_array);
}

unsigned int bit_reverse(unsigned int x)
{
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));

    return((x >> 16) | (x << 15));
}

void run_test(void *a_array, void *r_array, void *v_array, int minimum, int maximum, int samples, int repetitions, int copies, const char *desc, size_t size, CMPFUNC *cmpf)
{
	int cnt, rep;

	memcpy(v_array, r_array, maximum * size);

	for (rep = 0 ; rep < copies ; rep++)
	{
		memcpy((char *) r_array + rep * maximum * size, v_array, maximum * size);
	}
	quadsort(v_array, maximum, size, cmpf);

	for (cnt = 0 ; (size_t) cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, minimum, maximum, samples, repetitions, qsort, sorts[cnt], desc, size, cmpf);
	}
}

void range_test(int max, int samples, int repetitions, int seed)
{
	int cnt, last;
	int mem = max * 10 > 32768 * 64 ? max * 10 : 32768 * 64;
	char dist[40];

	int *a_array = (int *) malloc(max * sizeof(int));
	int *r_array = (int *) malloc(mem * sizeof(int));
	int *v_array = (int *) malloc(max * sizeof(int));

	srand(seed);

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = rand();
	}

	if (max <= 4096)
	{
		for (last = 1, samples = 32768*4, repetitions = 4 ; repetitions <= max ; repetitions *= 2, samples /= 2)
		{
			if (max >= repetitions)
			{
				sprintf(dist, "random %d-%d", last, repetitions);

				memcpy(v_array, r_array, repetitions * sizeof(int));
				quadsort(v_array, repetitions, sizeof(int), cmp_int);

				for (cnt = 0 ; (size_t) cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
				{
					test_sort(a_array, r_array, v_array, last, repetitions, 50, samples, qsort, sorts[cnt], dist, sizeof(int), cmp_int);
				}
				last = repetitions + 1;
			}
		}
		free(a_array);
		free(r_array);
		free(v_array);
		return;
	}

	if (max == 10000000)
	{
		repetitions = 10000000;

		for (max = 10 ; max <= 10000000 ; max *= 10)
		{
			repetitions /= 10;

			memcpy(v_array, r_array, max * sizeof(int));
			quadsort_prim(v_array, max, sizeof(int));

			sprintf(dist, "random %d", max);

			for (cnt = 0 ; (size_t) cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
			{
				test_sort(a_array, r_array, v_array, max, max, 10, repetitions, qsort, sorts[cnt], dist, sizeof(int), cmp_int);
			}
		}
	}
	else
	{
		for (samples = 32768*4, repetitions = 4 ; samples > 0 ; repetitions *= 2, samples /= 2)
		{
			if (max >= repetitions)
			{
				memcpy(v_array, r_array, repetitions * sizeof(int));
				quadsort(v_array, repetitions, sizeof(int), cmp_int);

				sprintf(dist, "random %d", repetitions);

				for (cnt = 0 ; (size_t) cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
				{
					test_sort(a_array, r_array, v_array, repetitions, repetitions, 100, samples, qsort, sorts[cnt], dist, sizeof(int), cmp_int);
				}
			}
		}
	}
	free(a_array);
	free(r_array);
	free(v_array);
	return;
}

#define VAR int

int main(int argc, char **argv)
{
	int max = 100000;
	int samples = 10;
	int repetitions = 1;
	int seed = 0;
	int cnt, mem;
	VAR *a_array, *r_array, *v_array, sum;

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

	seed = seed ? seed : time(NULL);

	printf("Info: int = %lu, long long = %lu, long double = %lu\n\n", sizeof(int) * 8, sizeof(long long) * 8, sizeof(long double) * 8);

	printf("Benchmark: array size: %d, samples: %d, repetitions: %d, seed: %d\n\n", max, samples, repetitions, seed);

	if (repetitions == 0)
	{
		range_test(max, samples, repetitions, seed);
		return 0;
	}

	mem = max * repetitions;

#ifndef SKIP_STRINGS
#ifndef cmp

	// C string

	{
		char **sa_array = (char **) malloc(max * sizeof(char **));
		char **sr_array = (char **) malloc(mem * sizeof(char **));
		char **sv_array = (char **) malloc(max * sizeof(char **));

		char *buffer = (char *) malloc(mem * 16);

		seed_rand(seed);

		for (cnt = 0 ; cnt < mem ; cnt++)
		{
			sprintf(buffer + cnt * 16, "%X", rand() % 1000000);

			sr_array[cnt] = buffer + cnt * 16;
		}
		run_test(sa_array, sr_array, sv_array, max, max, samples, repetitions, 0, "random string", sizeof(char **), cmp_str);

		free(sa_array);
		free(sr_array);
		free(sv_array);

		free(buffer);
	}

	// long double table

	{
		long double **da_array = (long double **) malloc(max * sizeof(long double *));
		long double **dr_array = (long double **) malloc(mem * sizeof(long double *));
		long double **dv_array = (long double **) malloc(max * sizeof(long double *));

		long double *buffer = (long double *) malloc(mem * sizeof(long double));

		if (da_array == NULL || dr_array == NULL || dv_array == NULL)
		{
			printf("main(%d,%d,%d): malloc: %s\n", max, samples, repetitions, strerror(errno));

			return 0;
		}

		seed_rand(seed);

		for (cnt = 0 ; cnt < mem ; cnt++)
		{
			buffer[cnt] = (long double) rand();
			buffer[cnt] += (long double) ((unsigned long long) rand() << 32ULL);

			dr_array[cnt] = buffer + cnt;
		}
		run_test(da_array, dr_array, dv_array, max, max, samples, repetitions, 0, "random double", sizeof(long double *), cmp_long_double_ptr);

		free(da_array);
		free(dr_array);
		free(dv_array);

		free(buffer);
	}

	// long long table

	{
		long long **la_array = (long long **) malloc(max * sizeof(long long *));
		long long **lr_array = (long long **) malloc(mem * sizeof(long long *));
		long long **lv_array = (long long **) malloc(max * sizeof(long long *));

		long long *buffer = (long long *) malloc(mem * sizeof(long long));

		if (la_array == NULL || lr_array == NULL || lv_array == NULL)
		{
			printf("main(%d,%d,%d): malloc: %s\n", max, samples, repetitions, strerror(errno));

			return 0;
		}

		seed_rand(seed);

		for (cnt = 0 ; cnt < mem ; cnt++)
		{
			buffer[cnt] = (long long) rand();
			buffer[cnt] += (long long) ((unsigned long long) rand() << 32ULL);

			lr_array[cnt] = buffer + cnt;
		}
		run_test(la_array, lr_array, lv_array, max, max, samples, repetitions, 0, "random long", sizeof(long long *), cmp_long_ptr);


		free(la_array);
		free(lr_array);
		free(lv_array);

		free(buffer);
	}

	// int table

	{
		int **la_array = (int **) malloc(max * sizeof(int *));
		int **lr_array = (int **) malloc(mem * sizeof(int *));
		int **lv_array = (int **) malloc(max * sizeof(int *));

		int *buffer = (int *) malloc(mem * sizeof(int));

		if (la_array == NULL || lr_array == NULL || lv_array == NULL)
		{
			printf("main(%d,%d,%d): malloc: %s\n", max, samples, repetitions, strerror(errno));

			return 0;
		}

		seed_rand(seed);

		for (cnt = 0 ; cnt < mem ; cnt++)
		{
			buffer[cnt] = rand();

			lr_array[cnt] = buffer + cnt;
		}
		run_test(la_array, lr_array, lv_array, max, max, samples, repetitions, 0, "random int", sizeof(int *), cmp_int_ptr);

		free(la_array);
		free(lr_array);
		free(lv_array);

		free(buffer);

		printf("\n");
	}
#endif
#endif
	// 128 bit

#ifndef SKIP_DOUBLES
	long double *da_array = (long double *) malloc(max * sizeof(long double));
	long double *dr_array = (long double *) malloc(mem * sizeof(long double));
	long double *dv_array = (long double *) malloc(max * sizeof(long double));

	if (da_array == NULL || dr_array == NULL || dv_array == NULL)
	{
		printf("main(%d,%d,%d): malloc: %s\n", max, samples, repetitions, strerror(errno));

		return 0;
	}

	seed_rand(seed);

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		dr_array[cnt] = (long double) rand();
		dr_array[cnt] += (long double) ((unsigned long long) rand() << 32ULL);
		dr_array[cnt] += 1.0L / 3.0L;
	}

	memcpy(dv_array, dr_array, max * sizeof(long double));
	quadsort(dv_array, max, sizeof(long double), cmp_long_double);

	for (cnt = 0 ; (size_t) cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(da_array, dr_array, dv_array, max, max, samples, repetitions, qsort, sorts[cnt], "random order", sizeof(long double), cmp_long_double);
	}

	test_sort(da_array, dr_array, dv_array, max, max, samples, repetitions, qsort, "s_quadsort", "random order", sizeof(long double), cmp_long_double_ptr);

	free(da_array);
	free(dr_array);
	free(dv_array);

	printf("\n");
#endif
	// 64 bit

#ifndef SKIP_LONGS
	long long *la_array = (long long *) malloc(max * sizeof(long long));
	long long *lr_array = (long long *) malloc(mem * sizeof(long long));
	long long *lv_array = (long long *) malloc(max * sizeof(long long));

	if (la_array == NULL || lr_array == NULL || lv_array == NULL)
	{
		printf("main(%d,%d,%d): malloc: %s\n", max, samples, repetitions, strerror(errno));

		return 0;
	}

	seed_rand(seed);

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		lr_array[cnt] = rand();
		lr_array[cnt] += (unsigned long long) rand() << 32ULL;
	}

	memcpy(lv_array, lr_array, max * sizeof(long long));
	quadsort(lv_array, max, sizeof(long long), cmp_long);

	for (cnt = 0 ; (size_t) cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(la_array, lr_array, lv_array, max, max, samples, repetitions, qsort, sorts[cnt], "random order", sizeof(long long), cmp_long);
	}

	free(la_array);
	free(lr_array);
	free(lv_array);

	printf("\n");
#endif
	// 32 bit

	a_array = (VAR *) malloc(max * sizeof(VAR));
	r_array = (VAR *) malloc(mem * sizeof(VAR));
	v_array = (VAR *) malloc(max * sizeof(VAR));

	int quad0 = 0;
	int nmemb = max;
	int half1 = nmemb / 2;
	int half2 = nmemb - half1;
	int quad1 = half1 / 2;
	int quad2 = half1 - quad1;
	int quad3 = half2 / 2;
	int quad4 = half2 - quad3;

	int span3 = quad1 + quad2 + quad3;

	// random

	seed_rand(seed);

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = rand();
	}
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, 0, "random order", sizeof(VAR), cmp_int);

	// random % 100

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = rand() % 100;
	}
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, 0, "random % 100", sizeof(VAR), cmp_int);

	// ascending

	for (cnt = sum = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = sum; sum += rand() % 5;
	}

	run_test(a_array, r_array, v_array, max, max, samples, repetitions, 0, "ascending order", sizeof(VAR), cmp_int);

	// ascending saw

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	quadsort(r_array + quad0, quad1, sizeof(VAR), cmp_int);
	quadsort(r_array + quad1, quad2, sizeof(VAR), cmp_int);
	quadsort(r_array + half1, quad3, sizeof(VAR), cmp_int);
	quadsort(r_array + span3, quad4, sizeof(VAR), cmp_int);

	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "ascending saw", sizeof(VAR), cmp_int);

	// pipe organ

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	quadsort(r_array + quad0, half1, sizeof(VAR), cmp_int);
	qsort(r_array + half1, half2, sizeof(VAR), cmp_rev);

	for (cnt = half1 + 1 ; cnt < max ; cnt++)
	{
		if (r_array[cnt] >= r_array[cnt - 1])
		{
			r_array[cnt] = r_array[cnt - 1] - 1; // guarantee the run is strictly descending
		}
	}

	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "pipe organ", sizeof(VAR), cmp_int);

	// descending

	for (cnt = 0, sum = mem * 10 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = sum; sum -= 1 + rand() % 5;
	}
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, 0, "descending order", sizeof(VAR), cmp_int);

	// descending saw

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	qsort(r_array + quad0, quad1, sizeof(VAR), cmp_rev);
	qsort(r_array + quad1, quad2, sizeof(VAR), cmp_rev);
	qsort(r_array + half1, quad3, sizeof(VAR), cmp_rev);
	qsort(r_array + span3, quad4, sizeof(VAR), cmp_rev);

	for (cnt = 1 ; cnt < max ; cnt++)
	{
		if (cnt == quad1 || cnt == half1 || cnt == span3) continue;

		if (r_array[cnt] >= r_array[cnt - 1])
		{
			r_array[cnt] = r_array[cnt - 1] - 1; // guarantee the run is strictly descending
		}
	}

	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "descending saw", sizeof(VAR), cmp_int);


	// random tail 25%

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}
	quadsort(r_array, span3, sizeof(VAR), cmp_int);

	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "random tail", sizeof(VAR), cmp_int);

	// random 50%

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}
	quadsort(r_array, half1, sizeof(VAR), cmp_int);

	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "random half", sizeof(VAR), cmp_int);

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
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, 0, "ascending tiles", sizeof(VAR), cmp_int);

	// bit-reversal

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = bit_reverse(cnt);
	}
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, 0, "bit reversal", sizeof(VAR), cmp_int);

#ifndef cmp
  #ifdef ANTIQSORT
    test_antiqsort;
  #endif
#endif

#define QUAD_DEBUG
#if __has_include("extra_tests.c")
  #include "extra_tests.c"
#endif

	free(a_array);
	free(r_array);
	free(v_array);

	return 0;
}
