Intro
-----

This document describes a stable non-recursive adaptive merge sort named quadsort.


The quad swap
-------------

At the core of quadsort is the quad swap. Traditionally most sorting
algorithms have been designed using the binary swap where two variables
are sorted using a third temporary variable. This typically looks as
following.
```c
    if (val[0] > val[1])
    {
        tmp[0] = val[0];
        val[0] = val[1];
        val[1] = tmp[0];
    }
```
Instead the quad swap sorts four variables using four swap variables.
During the first stage the four variables are partially sorted in the four
swap variables, in the second stage they are fully sorted back to the
original four variables.
```javascript
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
```c
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
```
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

Run detection
-------------
While there is no significant benefit to additional in order run detection
there is a notable benefit to reverse order run detection. Quadsort 1.1.3.1
implements reverse order run detection in the quad swap routine which obsoletes
the need for reverse order checks in the quad merge routine.

Boundary checks
---------------

Another issue with the traditional merge sort is that it performs wasteful
boundary checks. This looks as following:
```c
    while (a <= a_max && b <= b_max)
        if (a <= b)
            [insert a++]
        else
            [insert b++]
```
To optimize this quadsort compares the last element of sequence A against
the last element of sequence B. If the last element of sequence A is smaller
than the last element of sequence B we know that the (b < b_max) if check
will always be false because sequence A will be fully merged first.

Similarly if the last element of sequence A is greater than the last element
of sequence B we know that the (a < a_max) if check will always be false.
This looks as following:
```c
    if (val[a_max] <= val[b_max])
        while (a <= a_max)
        {
            while (a > b)
                [insert b++]
            [insert a++]
        }
    else
        while (b <= b_max)
        {
            while (a <= b)
                [insert a++]
            [insert b++]
        }
```
            

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

Wolfsort and flowsort
---------------------
[wolfsort](https://github.com/scandum/wolfsort) and flowsort are a hybrid radixsort / quadsort with improved performance on random data. They're mostly a proof of concept that only work on unsigned 32 and 64 bit integers.

Visualization
-------------

In the visualization below four tests are performed. The first test is on a random
distribution, the second on an ascending distribution, the third on a descending
distribution, and the fourth on an ascending distribution with a random tail.

The upper half shows the swap memory and the bottom half shows the main memory.
Colors are used to differentiate between skip, swap, merge, and copy operations.

![quadsort benchmark](https://github.com/scandum/quadsort/blob/master/benchmark.gif)

Benchmark: quadsort vs std::stable_sort vs timsort vs pdqsort vs wolfsort
-------------------------------------------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using g++ -O3 quadsort.cpp. Each test was ran 100 times
and only the best run is reported.

It should be noted that pdqsort is not a stable sort which is the reason it's much
faster on generic order data.

The X axis of the bar graph below is the execution time.

![Graph](/graph1.png)

<details><summary><b>quadsort vs std::stable_sort vs timsort vs pdqsort vs wolfsort data table</b></summary>

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|  quadsort |  1000000 |  i32 | 0.070469 | 0.070635 |             |     random order |
|stablesort |  1000000 |  i32 | 0.073865 | 0.074078 |             |     random order |
|   timsort |  1000000 |  i32 | 0.089192 | 0.089301 |             |     random order |
|   pdqsort |  1000000 |  i32 | 0.029911 | 0.029948 |             |     random order |
|  wolfsort |  1000000 |  i32 | 0.017359 | 0.017744 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.000485 | 0.000511 |             |        ascending |
|stablesort |  1000000 |  i32 | 0.010188 | 0.010261 |             |        ascending |
|   timsort |  1000000 |  i32 | 0.000331 | 0.000342 |             |        ascending |
|   pdqsort |  1000000 |  i32 | 0.000863 | 0.000875 |             |        ascending |
|  wolfsort |  1000000 |  i32 | 0.000539 | 0.000579 |             |        ascending |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.008901 | 0.008934 |             |    ascending saw |
|stablesort |  1000000 |  i32 | 0.017093 | 0.017275 |             |    ascending saw |
|   timsort |  1000000 |  i32 | 0.008615 | 0.008674 |             |    ascending saw |
|   pdqsort |  1000000 |  i32 | 0.047785 | 0.047921 |             |    ascending saw |
|  wolfsort |  1000000 |  i32 | 0.012490 | 0.012554 |             |    ascending saw |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.038260 | 0.038343 |             |    generic order |
|stablesort |  1000000 |  i32 | 0.042292 | 0.042388 |             |    generic order |
|   timsort |  1000000 |  i32 | 0.055855 | 0.055967 |             |    generic order |
|   pdqsort |  1000000 |  i32 | 0.008093 | 0.008168 |             |    generic order |
|  wolfsort |  1000000 |  i32 | 0.038320 | 0.038417 |             |    generic order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.000559 | 0.000576 |             | descending order |
|stablesort |  1000000 |  i32 | 0.010343 | 0.010438 |             | descending order |
|   timsort |  1000000 |  i32 | 0.000891 | 0.000900 |             | descending order |
|   pdqsort |  1000000 |  i32 | 0.001882 | 0.001897 |             | descending order |
|  wolfsort |  1000000 |  i32 | 0.000604 | 0.000625 |             | descending order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.006174 | 0.006245 |             |   descending saw |
|stablesort |  1000000 |  i32 | 0.014679 | 0.014767 |             |   descending saw |
|   timsort |  1000000 |  i32 | 0.006419 | 0.006468 |             |   descending saw |
|   pdqsort |  1000000 |  i32 | 0.016603 | 0.016629 |             |   descending saw |
|  wolfsort |  1000000 |  i32 | 0.006264 | 0.006329 |             |   descending saw |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.018675 | 0.018729 |             |      random tail |
|stablesort |  1000000 |  i32 | 0.026384 | 0.026508 |             |      random tail |
|   timsort |  1000000 |  i32 | 0.023226 | 0.023395 |             |      random tail |
|   pdqsort |  1000000 |  i32 | 0.028599 | 0.028674 |             |      random tail |
|  wolfsort |  1000000 |  i32 | 0.009517 | 0.009631 |             |      random tail |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.037593 | 0.037679 |             |      random half |
|stablesort |  1000000 |  i32 | 0.043755 | 0.043899 |             |      random half |
|   timsort |  1000000 |  i32 | 0.047008 | 0.047112 |             |      random half |
|   pdqsort |  1000000 |  i32 | 0.029800 | 0.029847 |             |      random half |
|  wolfsort |  1000000 |  i32 | 0.013238 | 0.013355 |             |      random half |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.009605 | 0.009673 |             |       wave order |
|stablesort |  1000000 |  i32 | 0.013667 | 0.013785 |             |       wave order |
|   timsort |  1000000 |  i32 | 0.007994 | 0.008138 |             |       wave order |
|   pdqsort |  1000000 |  i32 | 0.024683 | 0.024727 |             |       wave order |
|  wolfsort |  1000000 |  i32 | 0.009642 | 0.009709 |             |       wave order |
</details>

The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1). The
source code was compiled using g++ -O3 quadsort.cpp. Each test was ran 100 times and only
the best run is reported. It measures the performance on random data with array sizes
ranging from 256 to 1,048,576.

The X axis of the graph below is the number of elements, the Y axis is the execution time.

![Graph](/graph2.png)
<details><summary><b>quadsort vs std::stable_sort vs timsort vs pdqsort vs wolfsort vs flowsort data table</b></summary>

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|  quadsort |      256 |  i32 | 0.008306 | 0.009226 |             |     random order |
|stablesort |      256 |  i32 | 0.009325 | 0.022037 |             |     random order |
|   timsort |      256 |  i32 | 0.015605 | 0.026554 |             |     random order |
|   pdqsort |      256 |  i32 | 0.010840 | 0.015047 |             |     random order |
|  wolfsort |      256 |  i32 | 0.008287 | 0.008338 |             |     random order |
|  flowsort |      256 |  i32 | 0.008332 | 0.009783 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |      512 |  i32 | 0.007497 | 0.012202 |             |     random order |
|stablesort |      512 |  i32 | 0.014719 | 0.023305 |             |     random order |
|   timsort |      512 |  i32 | 0.014791 | 0.026926 |             |     random order |
|   pdqsort |      512 |  i32 | 0.010218 | 0.015399 |             |     random order |
|  wolfsort |      512 |  i32 | 0.005434 | 0.005711 |             |     random order |
|  flowsort |      512 |  i32 | 0.008525 | 0.008842 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |     1024 |  i32 | 0.012224 | 0.013265 |             |     random order |
|stablesort |     1024 |  i32 | 0.027686 | 0.033828 |             |     random order |
|   timsort |     1024 |  i32 | 0.033677 | 0.043642 |             |     random order |
|   pdqsort |     1024 |  i32 | 0.011030 | 0.015985 |             |     random order |
|  wolfsort |     1024 |  i32 | 0.005497 | 0.007087 |             |     random order |
|  flowsort |     1024 |  i32 | 0.008267 | 0.009476 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |     4096 |  i32 | 0.040918 | 0.041427 |             |     random order |
|stablesort |     4096 |  i32 | 0.043393 | 0.044039 |             |     random order |
|   timsort |     4096 |  i32 | 0.059069 | 0.059289 |             |     random order |
|   pdqsort |     4096 |  i32 | 0.019076 | 0.020721 |             |     random order |
|  wolfsort |     4096 |  i32 | 0.007710 | 0.009736 |             |     random order |
|  flowsort |     4096 |  i32 | 0.010430 | 0.012855 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |    16384 |  i32 | 0.051287 | 0.051480 |             |     random order |
|stablesort |    16384 |  i32 | 0.052293 | 0.052393 |             |     random order |
|   timsort |    16384 |  i32 | 0.068145 | 0.068304 |             |     random order |
|   pdqsort |    16384 |  i32 | 0.024337 | 0.024414 |             |     random order |
|  wolfsort |    16384 |  i32 | 0.012334 | 0.012433 |             |     random order |
|  flowsort |    16384 |  i32 | 0.015112 | 0.015173 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |    65536 |  i32 | 0.058787 | 0.058863 |             |     random order |
|stablesort |    65536 |  i32 | 0.060166 | 0.060262 |             |     random order |
|   timsort |    65536 |  i32 | 0.076500 | 0.076612 |             |     random order |
|   pdqsort |    65536 |  i32 | 0.026368 | 0.026425 |             |     random order |
|  wolfsort |    65536 |  i32 | 0.013164 | 0.013208 |             |     random order |
|  flowsort |    65536 |  i32 | 0.015327 | 0.015362 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |   262144 |  i32 | 0.066391 | 0.066484 |             |     random order |
|stablesort |   262144 |  i32 | 0.068144 | 0.068255 |             |     random order |
|   timsort |   262144 |  i32 | 0.084703 | 0.084835 |             |     random order |
|   pdqsort |   262144 |  i32 | 0.028397 | 0.028457 |             |     random order |
|  wolfsort |   262144 |  i32 | 0.013937 | 0.014095 |             |     random order |
|  flowsort |   262144 |  i32 | 0.016058 | 0.016107 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |  1048576 |  i32 | 0.074302 | 0.074495 |             |     random order |
|stablesort |  1048576 |  i32 | 0.076274 | 0.076419 |             |     random order |
|   timsort |  1048576 |  i32 | 0.093351 | 0.093517 |             |     random order |
|   pdqsort |  1048576 |  i32 | 0.030378 | 0.030446 |             |     random order |
|  wolfsort |  1048576 |  i32 | 0.034210 | 0.034403 |             |     random order |
|  flowsort |  1048576 |  i32 | 0.017668 | 0.017795 |             |     random order |
</details>

Benchmark: quadsort vs qsort (mergesort)
----------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using gcc -O3 quadsort.c. Each test was ran 100 times
and only the best run is reported. It's generated by running the benchmark using
1000000 100 1 as the argument.

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|  quadsort |  1000000 |  i32 | 0.095419 | 0.095829 |    19306163 |     random order |
|     qsort |  1000000 |  i32 | 0.102309 | 0.103419 |    18674256 |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.001988 | 0.002028 |      999999 |        ascending |
|     qsort |  1000000 |  i32 | 0.026712 | 0.027149 |     9884992 |        ascending |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.013601 | 0.013683 |     4007687 |    ascending saw |
|     qsort |  1000000 |  i32 | 0.034746 | 0.035097 |    10884987 |    ascending saw |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.061003 | 0.061243 |    19243695 |    generic order |
|     qsort |  1000000 |  i32 | 0.070227 | 0.071001 |    18619788 |    generic order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.001730 | 0.001787 |      999999 | descending order |
|     qsort |  1000000 |  i32 | 0.026025 | 0.026291 |    10066432 | descending order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.016891 | 0.017181 |     9519209 |   descending saw |
|     qsort |  1000000 |  i32 | 0.034630 | 0.034892 |    13906008 |   descending saw |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.027007 | 0.027321 |     6786142 |      random tail |
|     qsort |  1000000 |  i32 | 0.045876 | 0.046577 |    12249331 |      random tail |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.051911 | 0.052310 |    11382424 |      random half |
|     qsort |  1000000 |  i32 | 0.066441 | 0.067325 |    14529397 |      random half |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.027367 | 0.027626 |    15328606 |       wave order |
|     qsort |  1000000 |  i32 | 0.035065 | 0.035693 |    14656080 |       wave order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.044358 | 0.044636 |    15328606 |           stable |
|     qsort |  1000000 |  i32 | 0.048694 | 0.050377 |    14656080 |           stable |

In the benchmark above quadsort is compared against glibc qsort() using the same general
purpose interface and without any known unfair advantage, like inlining.

Benchmark: quadsort vs qsort (quicksort)
----------------------------------------
This particular test was performed using the qsort() implementation from Cygwin which uses
quicksort under the hood. The source code was compiled using gcc -O3 quadsort.c. Each test
was ran 100 times and only the best run is reported. It's generated by running the
benchmark using 1000000 100 1 as the argument.

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|  quadsort |  1000000 |  i32 | 0.118265 | 0.118730 |    19307249 |     random order |
|     qsort |  1000000 |  i32 | 0.141435 | 0.141914 |    20746525 |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.001972 | 0.002016 |      999999 |        ascending |
|     qsort |  1000000 |  i32 | 0.006877 | 0.006903 |     3000004 |        ascending |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.018578 | 0.018696 |     4007924 |    ascending saw |
|     qsort |  1000000 |  i32 | 0.074524 | 0.074710 |    20864688 |    ascending saw |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.074624 | 0.075161 |    19241311 |    generic order |
|     qsort |  1000000 |  i32 | 0.041052 | 0.041267 |     6554082 |    generic order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.001936 | 0.001955 |      999999 | descending order |
|     qsort |  1000000 |  i32 | 0.009379 | 0.009408 |     4000015 | descending order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.021063 | 0.021404 |     9519209 |   descending saw |
|     qsort |  1000000 |  i32 | 0.044183 | 0.044287 |    13152042 |   descending saw |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.033514 | 0.033777 |     6786589 |      random tail |
|     qsort |  1000000 |  i32 | 0.102907 | 0.103119 |    20494158 |      random tail |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.064992 | 0.065315 |    11382567 |      random half |
|     qsort |  1000000 |  i32 | 0.123339 | 0.123846 |    20607100 |      random half |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.035766 | 0.036136 |    15328606 |       wave order |
|     qsort |  1000000 |  i32 | 3.811840 | 3.811840 |  1974047339 |       wave order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.056537 | 0.056537 |    15328606 |           stable |
|     qsort |  1000000 |  i32 | 0.038299 | 0.038299 |    10333679 |         unstable |

In this benchmark it becomes clear why quicksort is often preferred above a traditional mergesort,
it has fewer comparisons for ascending, uniform, and descending order data. However, it performs
worse than quadsort on all tests except for generic order, which is due to the fact that quicksort
is an unstable sort. Quicksort also has an extremely poor sorting speed for wave order data.

Benchmark: quadsort vs qsort (mergesort) small arrays
-----------------------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using gcc -O3 quadsort.c. It's generated by running the
benchmark using 4096 0 0 as the argument.

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|  quadsort |        4 |  i32 | 0.000079 | 0.000079 |           5 |       random 1-4 |
|     qsort |        4 |  i32 | 0.000207 | 0.000221 |           4 |       random 1-4 |
|           |          |      |          |          |             |                  |
|  quadsort |        8 |  i32 | 0.000297 | 0.000337 |          20 |       random 5-8 |
|     qsort |        8 |  i32 | 0.000569 | 0.000650 |          16 |       random 5-8 |
|           |          |      |          |          |             |                  |
|  quadsort |       15 |  i32 | 0.000638 | 0.000655 |          29 |      random 9-15 |
|     qsort |       15 |  i32 | 0.001182 | 0.001581 |          23 |      random 9-15 |
|           |          |      |          |          |             |                  |
|  quadsort |       63 |  i32 | 0.001742 | 0.002205 |         130 |     random 16-63 |
|     qsort |       63 |  i32 | 0.003322 | 0.004559 |         114 |     random 16-63 |
|           |          |      |          |          |             |                  |
|  quadsort |      127 |  i32 | 0.002721 | 0.004109 |         803 |    random 64-127 |
|     qsort |      127 |  i32 | 0.005840 | 0.007227 |         726 |    random 64-127 |
|           |          |      |          |          |             |                  |
|  quadsort |      255 |  i32 | 0.003074 | 0.003995 |        1813 |   random 128-255 |
|     qsort |      255 |  i32 | 0.006958 | 0.007969 |        1643 |   random 128-255 |
|           |          |      |          |          |             |                  |
|  quadsort |      511 |  i32 | 0.003770 | 0.004671 |        4280 |   random 256-511 |
|     qsort |      511 |  i32 | 0.008305 | 0.009250 |        3962 |   random 256-511 |
|           |          |      |          |          |             |                  |
|  quadsort |     1023 |  i32 | 0.003759 | 0.004540 |        6930 |  random 512-1023 |
|     qsort |     1023 |  i32 | 0.007761 | 0.008444 |        6416 |  random 512-1023 |
|           |          |      |          |          |             |                  |
|  quadsort |     2047 |  i32 | 0.004581 | 0.005424 |       11525 | random 1024-2047 |
|     qsort |     2047 |  i32 | 0.007298 | 0.007864 |       10306 | random 1024-2047 |
|           |          |      |          |          |             |                  |
|  quadsort |     4095 |  i32 | 0.005938 | 0.006545 |       23605 | random 2048-4095 |
|     qsort |     4095 |  i32 | 0.007640 | 0.008040 |       20707 | random 2048-4095 |

