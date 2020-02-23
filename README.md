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

Instead the quad swap sorts four variables using four temporary variables.
During the first stage the four variables are partially sorted in the four
temporary variables, in the second stage they are fully sorted back to the
original four variables.
```
            [A]       [S] ╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴                  [A]🠆
               ＼   ／   ＼                   ＼             ／
                 (?)        ╴                    ╴         ╴
               ／   ＼        ＼                   ＼   ／
            [A]       [S]╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴        (?)╴╴╴╴╴╴╴[A]🠆
                         ＼        ＼         ＼   ／
                           (?)🠆      (?)🠆      ╴
                         ／        ／         ／   ＼
            [A]       [S]╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴        (?)╴╴╴╴╴╴╴[A]🠆
               ＼   ／        ／                   ／   ＼
                 (?)        ╴                    ╴         ╴
               ／   ＼   ／                   ／             ＼
            [A]       [S]╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴╴                  [A]🠆
```


This process is visualized in the diagram above.

After the first round of sorting a single if check determines if the four
temporary variables are sorted in order, if that's the case the swap
finishes up immediately. Next it checks if the temporary variables
are sorted in reverse-order, if that's the case the swap finishes up
immediately. If both checks fail the final arrangement is known and
two checks remain to determine the final order.

This eliminates 1 wasteful comparison for in-order sequences while creating
1 additional comparison for random sequences. However, in the real world we
are rarely comparing truly random data, so in any instance where data is
more likely to be orderly than disorderly this shift in probability will give
an advantage.

There should also be a slight overall performance increase due to the
elimination of wasteful swapping. In C the basic quad swap looks as
following:

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

In the case the array cannot be perfectly divided by 4 the tail, existing
of 1-3 elements, is sorted using the traditional swap.

quadsort
--------

In the first stage of quadsort the quad swap is used to pre-sort the
array into sorted 4-element blocks as described above.

The second stage uses an approach similar to the quad swap to detect
in-order and reverse-order arrangements, but as it's sorting blocks of 4 or
more elements the final step needs to be handled like the traditional merge
sort.

This can be visualized as following:

    main memory: AAAA BBBB CCCC DDDD

    swap memory: ABABABAB  CDCDCDCD

    main memory: ABCDABCDABCDABCD

In the first row quadswap has been used to create 4 blocks of 4 sorted
elements each. In the second row quadsort has been used to merge the blocks
into 2 blocks of 8 sorted elements each in swap memory. In the last row the
blocks are merged back to main memory and we're left with 1 block of 16
sorted elements. The following is a visualization.

![quadsort visualization](https://github.com/scandum/quadsort/blob/master/quadsort.gif)

These operations do require doubling the memory overhead for the swap space.

Skipping
--------

Another difference is that due to the increased cost of merge operations it
is beneficial to check whether the 4 blocks are in order or in reverse-order.
In the case of the 4 blocks being in order the merge operation is skipped,
as this would be pointless. This does however require an extra if check, and
for randomly sorted data this if check becomes increasingly unlikely to be
true as the block size increases. Fortunately the frequency of this if check
is quartered each loop, while the potential benefit is quadrupled each loop.

In the case only 2 out of 4 blocks are in order or in reverse-order the
comparisons in the merge itself are unnecessary and subsequently omitted.
The data still needs to be swapped but this is a less computational
intensive procedure.

This allows quadsort to sort in order and reverse-order sequences using
n + log n comparisons instead of n * log n comparisons.

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

Alignment
---------
When sorting an array of 65 elements you end up with a sorted array of 64
elements and a sorted array of 1 element in the end. Due to the ability to
skip this will result in no additional swap operation if the entire sequence
is in order. Regardless, if a program sorts in intervals it should pick an
optimal array size to do so.

A suboptimal array size is not disastrous for quadsort and it's outside of
the scope of this document to provide a solution.

Big O
-----

| Name | Best | Average | Worst | Stable | memory |
| ---- | ---- | ------- | ----- | ------ | ------ |
| quadsort | n | n log n | n log n | yes | n |

Quadsort makes n comparisons when the data is already sorted or reverse sorted.

Visualization
-------------

In the visualization below three tests are performed. The first test is on a random distribution, the second on a descending distribution, and the third on an ascending distribution with a random tail.

The upper half shows the swap memory and the bottom half shows the main memory. Colors are used to differentate between skip, swap, merge, and copy operations.

![quadsort benchmark](https://github.com/scandum/quadsort/blob/master/benchmark.gif)

Benchmark: quadsort vs qsort (mergesort)
----------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using gcc -O3 quadsort.c. Each test was ran 100 times
and only the best run is reported.

MO: lists the number of comparisons that are performed for 1 million items.

```
         quadsort: sorted 1000000 i64s in 0.092749 seconds. MO:   19287679 (random order)
            qsort: sorted 1000000 i64s in 0.105288 seconds. MO:   18674792 (random order)

         quadsort: sorted 1000000 i32s in 0.092766 seconds. MO:   19287679 (random order)
            qsort: sorted 1000000 i32s in 0.103551 seconds. MO:   18674792 (random order)

         quadsort: sorted 1000000 i32s in 0.001793 seconds. MO:     999999 (ascending order)
            qsort: sorted 1000000 i32s in 0.026766 seconds. MO:    9884992 (ascending order)

         quadsort: sorted 1000000 i32s in 0.001785 seconds. MO:     999999 (uniform order)
            qsort: sorted 1000000 i32s in 0.026788 seconds. MO:    9884992 (uniform order)

         quadsort: sorted 1000000 i32s in 0.004133 seconds. MO:    1416672 (descending order)
            qsort: sorted 1000000 i32s in 0.026163 seconds. MO:   10066432 (descending order)

         quadsort: sorted 1000000 i32s in 0.023856 seconds. MO:    6229390 (random tail)
            qsort: sorted 1000000 i32s in 0.043973 seconds. MO:   11832535 (random tail)

         quadsort: sorted 1000000 i32s in 0.024979 seconds. MO:   15371381 (wave order)
            qsort: sorted 1000000 i32s in 0.035257 seconds. MO:   14656080 (wave order)

         quadsort: sorted 1000000 i32s in 0.039972 seconds. MO:   15371381 (stable)
            qsort: sorted 1000000 i32s in 0.045825 seconds. MO:   14656080 (stable)

         quadsort: sorted    1000 i32s in 0.013171 seconds. KO:       9281 (random range)
            qsort: sorted    1000 i32s in 0.024601 seconds. KO:       8719 (random range)
```

In the benchmark above quadsort is compared against glibc qsort() using the same general
purpose interface and without any known unfair advantage.
```
     random order: 635, 202,  47, 229, etc
  ascending order: 1, 2, 3, 4, etc
    uniform order: 1, 1, 1, 1, etc
 descending order: 999, 998, 997, 996, etc
       wave order: 100, 1, 102, 2, 103, etc
  stable/unstable: checks whether the sort is stable or unstable
     random range: time to sort 1000 random order arrays with an average size of 500 items
```

Benchmark: quadsort vs qsort (quicksort)
----------------------------------------
This particular test was performed using the qsort() implementation from Cygwin which uses
quicksort under the hood. The source code was compiled using gcc -O3 quadsort.c. Each test
was ran 100 times and only the best run is reported.
```
         quadsort: sorted 1000000 i64s in 0.131264 seconds. MO:   19288965 (random order)
            qsort: sorted 1000000 i64s in 0.140029 seconds. MO:   20726941 (random order)

         quadsort: sorted 1000000 i32s in 0.117994 seconds. MO:   19288965 (random order)
            qsort: sorted 1000000 i32s in 0.131188 seconds. MO:   20726941 (random order)

         quadsort: sorted 1000000 i32s in 0.002190 seconds. MO:     999999 (ascending order)
            qsort: sorted 1000000 i32s in 0.007315 seconds. MO:    3000004 (ascending order)

         quadsort: sorted 1000000 i32s in 0.002163 seconds. MO:     999999 (uniform order)
            qsort: sorted 1000000 i32s in 0.002812 seconds. MO:    1000011 (uniform order)

         quadsort: sorted 1000000 i32s in 0.008784 seconds. MO:    1416672 (descending order)
            qsort: sorted 1000000 i32s in 0.009605 seconds. MO:    4000015 (descending order)

         quadsort: sorted 1000000 i32s in 0.033240 seconds. MO:    6229800 (random tail)
            qsort: sorted 1000000 i32s in 0.071783 seconds. MO:   20462567 (random tail)

         quadsort: sorted 1000000 i32s in 0.038756 seconds. MO:   15371381 (wave order)
            qsort: sorted 1000000 i32s in 4.437985 seconds. MO: 1974047339 (wave order)

         quadsort: sorted 1000000 i32s in 0.066367 seconds. MO:   15371381 (stable)
            qsort: sorted 1000000 i32s in 0.043977 seconds. MO:   10333679 (unstable)

         quadsort: sorted    1000 i32s in 0.015422 seconds. KO:       9281 (random range)
            qsort: sorted    1000 i32s in 0.031298 seconds. KO:      10276 (random range)
```
In this benchmark it becomes clear why quicksort is often preferred above a traditional mergesort,
it has fewer comparisons for ascending, uniform, and descending order data. However, it performs
worse than quadsort on all tests. Quicksort also has an extremely poor sorting speed for wave order data.

Benchmark: quadsort vs std::stable_sort
---------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using g++ -O3 quadsort.cpp. Each test was ran 100 times
and only the best run is reported.
```
         quadsort: sorted 1000000 i32s in 0.070503 seconds. (random order)
 std::stable_sort: sorted 1000000 i32s in 0.073255 seconds. (random order)

         quadsort: sorted 1000000 i32s in 0.000581 seconds. (ascending order)
 std::stable_sort: sorted 1000000 i32s in 0.008159 seconds. (ascending order)

         quadsort: sorted 1000000 i32s in 0.000585 seconds. (uniform order)
 std::stable_sort: sorted 1000000 i32s in 0.008150 seconds. (uniform order)

         quadsort: sorted 1000000 i32s in 0.002650 seconds. (descending order)
 std::stable_sort: sorted 1000000 i32s in 0.010440 seconds. (descending order)

         quadsort: sorted 1000000 i32s in 0.017025 seconds. (random tail)
 std::stable_sort: sorted 1000000 i32s in 0.022681 seconds. (random tail)

         quadsort: sorted 1000000 i32s in 0.010131 seconds. (wave order)
 std::stable_sort: sorted 1000000 i32s in 0.011656 seconds. (wave order)

         quadsort: sorted    1000 i32s in 0.009064 seconds. (random range)
 std::stable_sort: sorted    1000 i32s in 0.015343 seconds. (random range)
```
In this benchmark quadsort is compared against the c++ std::stable_sort without any
known advantages.
