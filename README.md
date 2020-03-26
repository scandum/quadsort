Intro
-----

This document describes a stable non-recursive adaptive merge sort named quadsort.


The quad swap
-------------

At the core of quadsort is the quad swap. Traditionally most sorting
algorithms have been designed using the binary swap where two variables
are sorted using a third temporary variable. This typically looks as
following.

    if (val[0] > val[1])
    {
        tmp[0] = val[0];
        val[0] = val[1];
        val[1] = tmp[0];
    }

Instead the quad swap sorts four variables using four swap variables.
During the first stage the four variables are partially sorted in the four
swap variables, in the second stage they are fully sorted back to the
original four variables.
```
            ╭─╮             ╭─╮                  ╭─╮          ╭─╮
            │A├─╮         ╭─┤S├────────┬─────────┤?├─╮    ╭───┤F│
            ╰─╯ │   ╭─╮   │ ╰─╯        │         ╰┬╯ │   ╭┴╮  ╰─╯
                ├───┤?├───┤            │       ╭──╯  ╰───┤?│
            ╭─╮ │   ╰─╯   │ ╭─╮        │       │         ╰┬╯  ╭─╮
            │A├─╯         ╰─┤S├────────│────────╮         ╰───┤F│
            ╰─╯             ╰┬╯        │       ││             ╰─╯
                            ╭┴╮ ╭─╮   ╭┴╮ ╭─╮  ││
                            │?├─┤F│   │?├─┤F│  ││
                            ╰┬╯ ╰─╯   ╰┬╯ ╰─╯  ││
            ╭─╮             ╭┴╮        │       ││             ╭─╮
            │A├─╮         ╭─┤S├────────│───────╯│         ╭───┤F│
            ╰─╯ │   ╭─╮   │ ╰─╯        │        ╰─╮      ╭┴╮  ╰─╯
                ├───┤?├───┤            │          │  ╭───┤?│
            ╭─╮ │   ╰─╯   │ ╭─╮        │         ╭┴╮ │   ╰┬╯  ╭─╮
            │A├─╯         ╰─┤S├────────┴─────────┤?├─╯    ╰───┤F│
            ╰─╯             ╰─╯                  ╰─╯          ╰─╯
```

This process is visualized in the diagram above.

After the first round of sorting a single if check determines if the four
swap variables are sorted in order, if that's the case the swap
finishes up immediately. Next it checks if the swap variables
are sorted in reverse-order, if that's the case the sort finishes up
immediately. If both checks fail the final arrangement is known and
two checks remain to determine the final order.

This eliminates 1 wasteful comparison for in-order sequences while creating
1 additional comparison for random sequences. However, in the real world we
are rarely comparing truly random data, so in any instance where data is
more likely to be orderly than disorderly this shift in probability will give
an advantage.

There is also an overall performance increase due to the elimination of
wasteful swapping. In C the basic quad swap looks as following:

    if (val[0] > val[1])
    {
        tmp[0] = val[1];
        tmp[1] = val[0];
    }
    else
    {
        tmp[0] = val[0];
        tmp[1] = val[1];
    }

    if (val[2] > val[3])
    {
        tmp[2] = val[3];
        tmp[3] = val[2];
    }
    else
    {
        tmp[2] = val[2];
        tmp[3] = val[3];
    }

    if (tmp[1] <= tmp[2])
    {
        val[0] = tmp[0];
        val[1] = tmp[1];
        val[2] = tmp[2];
        val[3] = tmp[3];
    }
    else if (tmp[0] > tmp[3])
    {
        val[0] = tmp[2];
        val[1] = tmp[3];
        val[2] = tmp[0];
        val[3] = tmp[1];
    }
    else
    {
       if (tmp[0] <= tmp[2])
       {
           val[0] = tmp[0];
           val[1] = tmp[2];
       }
       else
       {
           val[0] = tmp[2];
           val[1] = tmp[0];
       }

       if (tmp[1] <= tmp[3])
       {
           val[2] = tmp[1];
           val[3] = tmp[3];
       }
       else
       {
           val[2] = tmp[3];
           val[3] = tmp[1];
       }
    }

In the case the array cannot be perfectly divided by 4, the tail, existing
of 1-3 elements, is sorted using the traditional swap.

The quad swap above is implemented in-place in quadsort.

quad merge
----------

In the first stage of quadsort the quad swap is used to pre-sort the
array into sorted 4-element blocks as described above.

The second stage uses an approach similar to the quad swap to detect
in-order and reverse-order arrangements, but as it's sorting blocks of
4, 16, 64, or more elements, the final step needs to be handled like
the traditional merge sort.

This can be visualized as following:

    main memory: AAAA BBBB CCCC DDDD

    swap memory: ABABABAB  CDCDCDCD

    main memory: ABCDABCDABCDABCD

In the first row quad swap has been used to create 4 blocks of 4 sorted
elements each. In the second row quad merge has been used to merge the blocks
into 2 blocks of 8 sorted elements each in swap memory. In the last row the
blocks are merged back to main memory and we're left with 1 block of 16
sorted elements. The following is a visualization.

![quadsort visualization](https://github.com/scandum/quadsort/blob/master/quadsort.gif)

These operations do require doubling the memory overhead for the swap space.
More on this later.

Skipping
--------

Another difference is that due to the increased cost of merge operations it
is beneficial to check whether the 4 blocks are in order or in reverse-order.

In the case of the 4 blocks being in order the merge operation is skipped,
as this would be pointless. This does however require an extra if check, and
for randomly sorted data this if check becomes increasingly unlikely to be
true as the block size increases. Fortunately the frequency of this if check
is quartered each loop, while the potential benefit is quadrupled each loop.

In the case of the 4 blocks being in reverse order an in-place stable swap
is performed.

In the case only 2 out of 4 blocks are in order or in reverse-order the
comparisons in the merge itself are unnecessary and subsequently omitted.
The data still needs to be copied to swap memory, but this is a less
computational intensive procedure.

This allows quadsort to sort in order and reverse-order sequences using
n comparisons instead of n * log n comparisons.

Boundary checks
---------------

Another issue with the traditional merge sort is that it performs wasteful
boundary checks. This looks as following:

    if (a < a_max && b < b_max)
        if (a < b)
            [insert a]
        else
            [insert b]

To optimize this quadsort compares the last element of sequence A against
the last element of sequence B. If the last element of sequence A is smaller
than the last element of sequence B we know that the (b < b_max) if check
will always be false because sequence A will be fully merged first.

Similarly if the last element of sequence A is greater than the last element
of sequence B we know that the (a < a_max) if check will always be false.

tail merge
----------
When sorting an array of 65 elements you end up with a sorted array of 64
elements and a sorted array of 1 element in the end. Due to the ability to
skip this will result in no additional swap operation if the entire sequence
is in order. Regardless, if a program sorts in intervals it should pick an
optimal array size (64, 256, or 1024) to do so.

Another problem is that a sub-optimal array results in wasteful swapping. To
work around these two problems the quad merge routine is aborted when the
block size reaches 1/8th of the array size, and the remainder of the array
is sorted using a tail merge.

The main advantage of the tail merge is that it allows reducing the swap
space of quadsort to n / 2 without notably impacting performance.

Big O
-----

| Name | Best | Average | Worst | Stable | Memory |
| ---- | ---- | ------- | ----- | ------ | ------ |
| quadsort | n | n log n | n log n | yes | n |

Quadsort makes n comparisons when the data is already sorted or reverse sorted.

Cache
-----
Because quadsort uses n / 2 swap memory its cache utilization is not as ideal as
in-place sorts. However, in-place sorting of random data results in suboptimal
swapping. Based on my benchmarks it appears that quadsort is always faster than
in-place sorts for array sizes that do not exhaust the L1 cache, which can be up
to 64KB on modern processors.

Wolfsort
--------
[wolfsort](https://github.com/scandum/wolfsort) is a hybrid radixsort / quadsort which improves performance on random data. It's mostly a proof of concept that only works on unsigned 32 and 64 bit integers.

Visualization
-------------

In the visualization below four tests are performed. The first test is on a random
distribution, the second on an ascending distribution, the third on a descending
distribution, and the fourth on an ascending distribution with a random tail.

The upper half shows the swap memory and the bottom half shows the main memory.
Colors are used to differentate between skip, swap, merge, and copy operations.

![quadsort benchmark](https://github.com/scandum/quadsort/blob/master/benchmark.gif)

Benchmark: quadsort vs qsort (mergesort)
----------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using gcc -O3 quadsort.c. Each test was ran 100 times
and only the best run is reported.

MO: lists the number of comparisons that are performed for 1 million items.

```
         quadsort: sorted 1000000 i32s in 0.092585 seconds. MO:   19287676 (random order)
            qsort: sorted 1000000 i32s in 0.103579 seconds. MO:   18674792 (random order)

         quadsort: sorted 1000000 i32s in 0.001785 seconds. MO:     999999 (ascending order)
            qsort: sorted 1000000 i32s in 0.026794 seconds. MO:    9884992 (ascending order)

         quadsort: sorted 1000000 i32s in 0.001805 seconds. MO:     999999 (uniform order)
            qsort: sorted 1000000 i32s in 0.026785 seconds. MO:    9884992 (uniform order)

         quadsort: sorted 1000000 i32s in 0.004543 seconds. MO:    1416674 (descending order)
            qsort: sorted 1000000 i32s in 0.026317 seconds. MO:   10066432 (descending order)

         quadsort: sorted 1000000 i32s in 0.024400 seconds. MO:    6701332 (random tail)
            qsort: sorted 1000000 i32s in 0.043973 seconds. MO:   11832535 (random tail)

         quadsort: sorted 1000000 i32s in 0.026678 seconds. MO:   15322807 (wave order)
            qsort: sorted 1000000 i32s in 0.035587 seconds. MO:   14656080 (wave order)

         quadsort: sorted 1000000 i32s in 0.040201 seconds. MO:   15322807 (stable)
            qsort: sorted 1000000 i32s in 0.046046 seconds. MO:   14656080 (stable)

         quadsort: sorted    1000 i32s in 0.012834 seconds. KO:       9279 (random range)
            qsort: sorted    1000 i32s in 0.024238 seconds. KO:       8719 (random range)
```
In the benchmark above quadsort is compared against glibc qsort() using the same general
purpose interface and without any known unfair advantage, like inlining.
```
     random order: 635, 202,  47, 229, etc
  ascending order: 1, 2, 3, 4, etc
    uniform order: 1, 1, 1, 1, etc
 descending order: 999, 998, 997, 996, etc
       wave order: 100, 1, 102, 2, 103, 3, etc
  stable/unstable: 100, 1, 102, 1, 103, 1, etc
     random range: time to sort 1000 arrays ranging from size 0 to 999 containing random data
```

Benchmark: quadsort vs qsort (quicksort)
----------------------------------------
This particular test was performed using the qsort() implementation from Cygwin which uses
quicksort under the hood. The source code was compiled using gcc -O3 quadsort.c. Each test
was ran 100 times and only the best run is reported.
```
         quadsort: sorted 1000000 i32s in 0.116897 seconds. MO:   19288966 (random order)
            qsort: sorted 1000000 i32s in 0.130684 seconds. MO:   20726941 (random order)

         quadsort: sorted 1000000 i32s in 0.002040 seconds. MO:     999999 (ascending order)
            qsort: sorted 1000000 i32s in 0.007183 seconds. MO:    3000004 (ascending order)

         quadsort: sorted 1000000 i32s in 0.002034 seconds. MO:     999999 (uniform order)
            qsort: sorted 1000000 i32s in 0.002799 seconds. MO:    1000011 (uniform order)

         quadsort: sorted 1000000 i32s in 0.007490 seconds. MO:    1416674 (descending order)
            qsort: sorted 1000000 i32s in 0.009457 seconds. MO:    4000015 (descending order)

         quadsort: sorted 1000000 i32s in 0.031719 seconds. MO:    6704940 (random tail)
            qsort: sorted 1000000 i32s in 0.071398 seconds. MO:   20462567 (random tail)

         quadsort: sorted 1000000 i32s in 0.037423 seconds. MO:   15322807 (wave order)
            qsort: sorted 1000000 i32s in 4.445371 seconds. MO: 1974047339 (wave order)

         quadsort: sorted 1000000 i32s in 0.065062 seconds. MO:   15322807 (stable)
            qsort: sorted 1000000 i32s in 0.044099 seconds. MO:   10333679 (unstable)

         quadsort: sorted    1000 i32s in 0.015201 seconds. KO:       9279 (random range)
            qsort: sorted    1000 i32s in 0.031053 seconds. KO:      10276 (random range)
```
In this benchmark it becomes clear why quicksort is often preferred above a traditional mergesort,
it has fewer comparisons for ascending, uniform, and descending order data. However, it performs
worse than quadsort on all tests. Quicksort also has an extremely poor sorting speed for wave order data.
The random range test shows quadsort to be more than twice as fast when sorting small arrays.

Benchmark: quadsort vs std::stable_sort vs timsort vs pdqsort vs wolfsort
-------------------------------------------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using g++ -O3 quadsort.cpp. Each test was ran 100 times
and only the best run is reported.
```
         quadsort: sorted 1000000 i32s in 0.071135 seconds.  (random order)
       stablesort: sorted 1000000 i32s in 0.072644 seconds.  (random order)
          timsort: sorted 1000000 i32s in 0.087601 seconds.  (random order)
          pdqsort: sorted 1000000 i32s in 0.030005 seconds.  (random order)
         wolfsort: sorted 1000000 i32s in 0.021071 seconds.  (random order)

         quadsort: sorted 1000000 i32s in 0.000493 seconds.  (ascending)
       stablesort: sorted 1000000 i32s in 0.010274 seconds.  (ascending)
          timsort: sorted 1000000 i32s in 0.000334 seconds.  (ascending)
          pdqsort: sorted 1000000 i32s in 0.000864 seconds.  (ascending)
         wolfsort: sorted 1000000 i32s in 0.000523 seconds.  (ascending)

         quadsort: sorted 1000000 i32s in 0.009052 seconds.  (ascending saw)
       stablesort: sorted 1000000 i32s in 0.017162 seconds.  (ascending saw)
          timsort: sorted 1000000 i32s in 0.008472 seconds.  (ascending saw)
          pdqsort: sorted 1000000 i32s in 0.052092 seconds.  (ascending saw)
         wolfsort: sorted 1000000 i32s in 0.015734 seconds.  (ascending saw)

         quadsort: sorted 1000000 i32s in 0.039240 seconds.  (generic order)
       stablesort: sorted 1000000 i32s in 0.041762 seconds.  (generic order)
          timsort: sorted 1000000 i32s in 0.054422 seconds.  (generic order)
          pdqsort: sorted 1000000 i32s in 0.007902 seconds.  (generic order)
         wolfsort: sorted 1000000 i32s in 0.039278 seconds.  (generic order)

         quadsort: sorted 1000000 i32s in 0.000559 seconds.  (descending order)
       stablesort: sorted 1000000 i32s in 0.010405 seconds.  (descending order)
          timsort: sorted 1000000 i32s in 0.000750 seconds.  (descending order)
          pdqsort: sorted 1000000 i32s in 0.001885 seconds.  (descending order)
         wolfsort: sorted 1000000 i32s in 0.000578 seconds.  (descending order)

         quadsort: sorted 1000000 i32s in 0.006794 seconds.  (descending saw)
       stablesort: sorted 1000000 i32s in 0.014028 seconds.  (descending saw)
          timsort: sorted 1000000 i32s in 0.006202 seconds.  (descending saw)
          pdqsort: sorted 1000000 i32s in 0.016589 seconds.  (descending saw)
         wolfsort: sorted 1000000 i32s in 0.006839 seconds.  (descending saw)

         quadsort: sorted 1000000 i32s in 0.018863 seconds.  (random tail)
       stablesort: sorted 1000000 i32s in 0.026095 seconds.  (random tail)
          timsort: sorted 1000000 i32s in 0.022851 seconds.  (random tail)
          pdqsort: sorted 1000000 i32s in 0.028845 seconds.  (random tail)
         wolfsort: sorted 1000000 i32s in 0.011080 seconds.  (random tail)

         quadsort: sorted 1000000 i32s in 0.037896 seconds.  (random half)
       stablesort: sorted 1000000 i32s in 0.043151 seconds.  (random half)
          timsort: sorted 1000000 i32s in 0.046088 seconds.  (random half)
          pdqsort: sorted 1000000 i32s in 0.029575 seconds.  (random half)
         wolfsort: sorted 1000000 i32s in 0.015783 seconds.  (random half)

         quadsort: sorted 1000000 i32s in 0.010013 seconds.  (wave order)
       stablesort: sorted 1000000 i32s in 0.013475 seconds.  (wave order)
          timsort: sorted 1000000 i32s in 0.009149 seconds.  (wave order)
          pdqsort: sorted 1000000 i32s in 0.024758 seconds.  (wave order)
         wolfsort: sorted 1000000 i32s in 0.010016 seconds.  (wave order)

         quadsort: sorted    1023 i32s in 0.005159 seconds. (random 1-1023)
       stablesort: sorted    1023 i32s in 0.012832 seconds. (random 1-1023)
          timsort: sorted    1023 i32s in 0.014249 seconds. (random 1-1023)
          pdqsort: sorted    1023 i32s in 0.010355 seconds. (random 1-1023)
         wolfsort: sorted    1023 i32s in 0.005377 seconds. (random 1-1023)
```
In this benchmark quadsort is compared without any known advantages. It should be noted
that pdqsort is not a stable sort which is the  reason it's faster on generic order data.

The final random range test shows quadsort to be aproximately three times faster than Timsort and two
times faster than pdqsort for small arrays containing random data.
