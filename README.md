Intro
-----

This document describes a stable bottom-up adaptive branchless merge sort named quadsort. A [visualisation](https://github.com/scandum/quadsort#visualization) and [benchmarks](https://github.com/scandum/quadsort#benchmark-quadsort-vs-stdstable_sort-vs-timsort) are available at the bottom.

The quad swap analyzer
----------------------
Quadsort starts out with an analyzer that has the following tasks:

1. Detect ordered data with minimal comparisons.
2. Detect reverse order data with minimal comparisons.
3. Do the above without impacting performance on random data.
4. Exit the quad swap analyzer with sorted blocks of 8 elements.

Ordered data handling
---------------------
Quadsort's analyzer examines the array 8 elements at a time. It performs 4
comparisons on elements (1,2), (3,4), (5,6), and (7,8) of which the result
is stored and a bitmask is created with a value between 0 and 15 for all
the possible combinations. If the result is 0 it means the 4 comparisons
were all in order.

What remains is 3 more comparisons on elements (2,3), (4,5), and (6,7) to
determine if all 8 elements are in order. Traditional sorts would
do this with 7 branched individual comparisons, which would result in 3.5
branch mispredictions on random data on average. Using quadsort's method
results in 0.2 branch mispredictions on random data on average.

If the data is in order quadsort moves on to the next 8 elements. If the data turns
out to be neither in order or in reverse order, 4 branchless swaps are performed
using the stored comparison results, followed by a branchless parity merge. More on
that later.

Reverse order handling
----------------------
Reverse order data is typically moved using a simple reversal function, as following.
```c
int reverse(int array[], int start, int end, int swap)
{
    while (start < end)
    {
        swap = array[start];
        array[start++] = array[end];
        array[end--] = swap;
    }
}
```
While random data can only be sorted using **n log n** comparisons and
**n log n** moves, reverse-order data can be sorted using **n** comparisons
and **n** moves through run detection and a reversal routine. Without run
detection the best you can do is sort it in **n** comparisons and **n log n** moves. 

Run detection, as the name implies, comes with a detection cost. Thanks
to the laws of probability a quad swap can cheat however. The chance of
4 separate pairs of elements being in reverse order is 1 in 16. So there's
a 6.25% chance quadsort makes a wasteful run check.

What about run detection for in-order data? While we're turning
**n log n** moves into **n** moves with reverse order run detection, we'd be
turning **0** moves into **0** moves with forward run detection. So there's
no point in doing so.

The next optimization is to write the quad swap analyzer in such a way that
we can perform a simple check to see if the entire array was in reverse order,
if so, the sort is finished.

At the end of the loop the array has been turned into a series of ordered
blocks of 8 elements.

Ping-Pong Quad Merge
--------------------
Most textbook mergesort examples merge two blocks to swap memory, then copy
them back to main memory.
```
main memory ┌────────┐┌────────┐
            └────────┘└────────┘
                  ↓ merge ↓
swap memory ┌──────────────────┐
            └──────────────────┘
                  ↓ copy ↓
main memory ┌──────────────────┐
            └──────────────────┘
```
This doubles the amount of moves and we can fix this by merging 4 blocks at once
using a quad merge / ping-pong merge like so:
```
main memory ┌────────┐┌────────┐┌────────┐┌────────┐
            └────────┘└────────┘└────────┘└────────┘
                  ↓ merge ↓           ↓ merge ↓
swap memory ┌──────────────────┐┌──────────────────┐
            └──────────────────┘└──────────────────┘
                            ↓ merge ↓
main memory ┌──────────────────────────────────────┐
            └──────────────────────────────────────┘
```

It is possible to interleave the two merges to swap memory for increased memory-level
parallelism, but this can both increase and decrease performance.

Skipping
--------
Just like with the quad swap it is beneficial to check whether the 4 blocks
are in-order.

In the case of the 4 blocks being in-order the merge operation is skipped,
as this would be pointless. Because reverse order data is handled in the
quad swap there is no need to check for reverse order blocks.

This allows quadsort to sort in-order sequences using **n** comparisons instead
of **n * log n** comparisons.

Parity merge
------------
A parity merge takes advantage of the fact that if you have two n length arrays,
you can fully merge the two arrays by performing n merge operations on the start
of each array, and n merge operations on the end of each array. The arrays must
be of exactly equal length.

The main advantage of a parity merge over a traditional merge is that the loop
of a parity merge can be fully unrolled.

If the arrays are not of equal length a hybrid parity merge can be performed. One
way to do so is using n parity merges where n is the size of the smaller array,
before switching to a traditional merge.

Branchless parity merge
-----------------------
Since the parity merge can be unrolled it's very suitable for branchless
optimizations to speed up the sorting of random data. Another advantage
is that two separate memory regions are accessed in the same loop, allowing
memory-level parallelism. This makes the routine up to 2.5 times faster for
random data on most hardware.

Increasing the memory regions from two to four can result in both performance
gains and performance losses.

The following is a visualization of an array with 256 random elements getting
turned into sorted blocks of 32 elements using ping-pong parity merges.

![quadsort visualization](/images/quadswap.gif)

Quad galloping merge
--------------------
While a branchless parity merge sorts random data faster, it sorts ordered data
slower. One way to solve this problem is by using a method with a resemblance
to the galloping merge concept first introduced by timsort.

The quad galloping merge works in a similar way to the quad swap.
Instead of merging the ends of two arrays two items at a time, it merges
four items at a time.
```
┌───┐┌───┐┌───┐    ┌───┐┌───┐┌───┐            ╭───╮  ┌───┐┌───┐┌───┐
│ A ││ B ││ C │    │ X ││ Y ││ Z │        ┌───│B<X├──┤ A ││ B ││C/X│
└─┬─┘└─┬─┘└───┘    └─┬─┘└─┬─┘└───┘        │   ╰─┬─╯  └───┘└───┘└───┘
  └────┴─────────────┴────┴───────────────┘     │  ╭───╮  ┌───┐┌───┐┌───┐
                                                └──│A>Y├──┤ X ││ Y ││A/Z│
                                                   ╰─┬─╯  └───┘└───┘└───┘
                                                     │    ┌───┐┌───┐┌───┐
                                                     └────│A/X││X/A││B/Y│
                                                          └───┘└───┘└───┘
```
When merging ABC and XYZ it first checks if B is smaller or equal to X. If
that's the case A and B are copied to swap. If not, it checks if A is greater
than Y. If that's the case X and Y are copied to swap.

If either check is false it's known that the two remaining distributions
are A X and X A. This allows performing an optimal branchless merge. Since
it's known each block still has at least 1 item remaining (B and Y) and
there is a high probability of the data to be random, another (sub-optimal)
branchless merge can be performed.

In C this looks as following:
```c
while (l < l_size - 1 && r < r_size - 1)
{
    if (left[l + 1] <= right[r])
    {
        swap[s++] = left[l++];
        swap[s++] = left[l++];
    }
    else if (left[l] > right[r + 1])
    {
        swap[s++] = right[r++];
        swap[s++] = right[r++];
    }
    else
    {
        a = left[l] > right[r];
        x = !a;
        swap[s + a] = left[l++];
        swap[s + x] = right[r++];
        s += 2;
    }
}
```
Overall the quad galloping merge gives a decent performance gain for both ordered
and random data, particularly when the two arrays are of unequal length. When
two arrays are of equal length quadsort looks 8 elements ahead, and performs an
8 element parity merge if it can't skip ahead.

Merge strategy
--------------
Quadsort will merge blocks of 8 into blocks of 32, which it will merge into
blocks of 128, 512, 2048, 8192, etc.

For each ping-pong merge quadsort will perform two comparisons to see if it will be faster
to use a parity merge or a quad galloping merge, and pick the best option.

Tail merge
----------
When sorting an array of 33 elements you end up with a sorted array of 32
elements and a sorted array of 1 element in the end. If a program sorts in
intervals it should pick an optimal array size (32, 128, 512, etc) to do so.

To minimalize the impact the remainder of the array is sorted using a tail
merge.

The main advantage of a tail merge is that it allows reducing the swap
space of quadsort to **n / 2** and that the galloping merge strategy works best
on arrays of different lengths. It also greatly simplifies the ping-pong
quad merge routine which only needs to work on arrays of equal length.

Rotate merge
------------
By using rotations the swap space of quadsort is reduced further from **n / 2**
to **n / 4**. Rotations can be performed with minimal performance loss by using
[monobound binary searches](https://github.com/scandum/binary_search) and [trinity / bridge rotations](https://github.com/scandum/rotate).

Big O
-----
```cobol
                 ┌───────────────────────┐┌───────────────────────┐
                 │comparisons            ││swap memory            │
┌───────────────┐├───────┬───────┬───────┤├───────┬───────┬───────┤┌──────┐┌─────────┐┌─────────┐
│name           ││min    │avg    │max    ││min    │avg    │max    ││stable││partition││adaptive │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│mergesort      ││n log n│n log n│n log n││n      │n      │n      ││yes   ││no       ││no       │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│quadsort       ││n      │n log n│n log n││1      │n      │n      ││yes   ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│quicksort      ││n log n│n log n│n²     ││1      │1      │1      ││no    ││yes      ││no       │
└───────────────┘└───────┴───────┴───────┘└───────┴───────┴───────┘└──────┘└─────────┘└─────────┘
```
Quadsort makes n comparisons when the data is fully sorted or reverse sorted.

Data Types
----------
The C implementation of quadsort supports long doubles and 8, 16, 32, and 64 bit data types. By using pointers it's possible to sort any other data type, like strings.

Interface
---------
Quadsort uses the same interface as qsort, which is described in [man qsort](https://man7.org/linux/man-pages/man3/qsort.3p.html).

In addition to supporting `(l - r)` and `((l > r) - (l < r))` for the comparison function, `(l > r)` is valid as well. Special note should be taken that C++ sorts use `(l < r)` for the comparison function, which is incompatible with the C standard. When porting quadsort to C++ or Rust, switch `(l, r)` to `(r, l)` for every comparison.

Quadsort also comes with the `quadsort_prim(void *array, size_t nmemb, size_t size)` function to perform primitive comparisons on arrays of 32 and 64 bit integers. Nmemb is the number of elements, while size should be either `sizeof(int)` or `sizeof(long long)` for signed integers, and `sizeof(int) + 1` or `sizeof(long long) + 1` for unsigned integers. Support for the char, short, float, double, and long double types can be easily added in quadsort.h.

Memory
------
By default quadsort uses between n and n / 4 swap memory. If memory allocation fails quadsort will switch to sorting in-place through rotations. The minimum memory requirement is 32 elements of stack memory.

Performance
-----------
Quadsort is one of the fastest merge sorts written to date. It is faster than quicksort for most data distributions, with the notable exception of generic data. Data type is important as well, and overall quadsort is faster for sorting referenced objects.

Compared to Timsort, Quadsort has similar overall adaptivity while being much faster on random data, even without branchless optimizations.

Quicksort has an overall advantage as the array size increases beyond the L1 cache, which could be due to more efficient partitioning. For small arrays quadsort has a significant advantage due to quicksort's inability to cost effectively pick a reliable pivot.

To take full advantage of branchless operations the cmp macro needs to be uncommented in bench.c, which will increase the performance by 30% on primitive types. The quadsort_prim function can be used to access primitive comparisons directly. 

Variants
--------
- [blitsort](https://github.com/scandum/blitsort) is a hybrid stable in-place rotate quicksort / quadsort.

- [crumsort](https://github.com/scandum/crumsort) is a hybrid unstable in-place quicksort / quadsort.

- [fluxsort](https://github.com/scandum/fluxsort) is a hybrid stable quicksort / quadsort.

- [gridsort](https://github.com/scandum/gridsort) is a hybrid stable cubesort / quadsort. Gridsort is an online sort and might be of interest to those interested in data structures and sorting very large arrays.

- [twinsort](https://github.com/scandum/twinsort) is a simplified quadsort with a
much smaller code size. Twinsort might be of use to people who want to port or understand quadsort; it does not use
pointers or gotos. It is a bit dated and isn't branchless.

- [piposort](https://github.com/scandum/piposort) is a simplified branchless quadsort with a much smaller code size and complexity while still being very fast. Piposort might be of use to people who want to port quadsort. This is a lot easier when you start out small.

- [wolfsort](https://github.com/scandum/wolfsort) is a hybrid stable radixsort / fluxsort with improved performance on random data. It's mostly a proof of concept that only works on unsigned 32 bit integers.

- [Robin Hood Sort](https://github.com/mlochbaum/rhsort) is a hybrid stable radixsort / dropsort with improved performance on random and generic data. It has a compilation option to use quadsort for its merging.

Credits
-------
I personally invented the quad swap analyzer, quad galloping merge, parity merge, branchless parity merge,
monobound binary search, bridge rotation, and trinity rotation.

The ping-pong quad merge had been independently implemented in wikisort prior to quadsort, and
likely by others as well.

The monobound binary search has been independently implemented, often referred to as a branchless binary search. I published a working concept in 2014, which appears to pre-date others.

Special kudos to [Musiccombo and Co](https://www.youtube.com/c/Musicombo) for getting me interested in rotations and branchless logic.

Visualization
-------------
In the visualization below nine tests are performed on 256 elements.

1. Random order
2. Ascending order
3. Ascending Saw
4. Generic random order
5. Descending order
6. Descending Saw
7. Random tail
8. Random half
9. Ascending tiles.

The upper half shows the swap memory and the bottom half shows the main memory.
Colors are used to differentiate various operations. Quad swaps are in cyan, reversals in magenta, skips in green, parity merges in orange, bridge rotations in yellow, and trinity rotations are in violet.

[![quadsort benchmark](/images/quadsort.gif)](https://www.youtube.com/watch?v=GJjH_99BS70)

The [visualization is available on YouTube](https://www.youtube.com/watch?v=GJjH_99BS70) and there's also a [YouTube video of a java port of quadsort](https://www.youtube.com/watch?v=drSeVadf05M) in [ArrayV](https://github.com/Gaming32/ArrayV-v4.0) on a wide variety of data distributions.

Benchmark: quadsort vs std::stable_sort vs timsort
--------------------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. Stablesort is g++'s std:stablesort function. Each test was ran 100 times
on 100,000 elements. A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph1.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |   100000 |  128 | 0.010998 | 0.011096 |         0 |     100 |     random order |
|  quadsort |   100000 |  128 | 0.009800 | 0.009888 |         0 |     100 |     random order |
|   timsort |   100000 |  128 | 0.012655 | 0.012742 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |   100000 |   64 | 0.006054 | 0.006128 |         0 |     100 |     random order |
|  quadsort |   100000 |   64 | 0.002507 | 0.002528 |         0 |     100 |     random order |
|   timsort |   100000 |   64 | 0.007761 | 0.007806 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |   100000 |   32 | 0.006110 | 0.006160 |         0 |     100 |     random order |
|  quadsort |   100000 |   32 | 0.002452 | 0.002469 |         0 |     100 |     random order |
|   timsort |   100000 |   32 | 0.007620 | 0.007660 |         0 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003956 | 0.003989 |         0 |     100 |     random % 100 |
|  quadsort |   100000 |   32 | 0.001791 | 0.001805 |         0 |     100 |     random % 100 |
|   timsort |   100000 |   32 | 0.005625 | 0.005660 |         0 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000821 | 0.000840 |         0 |     100 |  ascending order |
|  quadsort |   100000 |   32 | 0.000052 | 0.000052 |         0 |     100 |  ascending order |
|   timsort |   100000 |   32 | 0.000045 | 0.000045 |         0 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001511 | 0.001533 |         0 |     100 |    ascending saw |
|  quadsort |   100000 |   32 | 0.000614 | 0.000618 |         0 |     100 |    ascending saw |
|   timsort |   100000 |   32 | 0.000841 | 0.000849 |         0 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000901 | 0.000920 |         0 |     100 |       pipe organ |
|  quadsort |   100000 |   32 | 0.000269 | 0.000271 |         0 |     100 |       pipe organ |
|   timsort |   100000 |   32 | 0.000179 | 0.000184 |         0 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000930 | 0.000950 |         0 |     100 | descending order |
|  quadsort |   100000 |   32 | 0.000059 | 0.000059 |         0 |     100 | descending order |
|   timsort |   100000 |   32 | 0.000101 | 0.000101 |         0 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001510 | 0.001520 |         0 |     100 |   descending saw |
|  quadsort |   100000 |   32 | 0.000615 | 0.000619 |         0 |     100 |   descending saw |
|   timsort |   100000 |   32 | 0.000842 | 0.000852 |         0 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.002158 | 0.002188 |         0 |     100 |      random tail |
|  quadsort |   100000 |   32 | 0.000813 | 0.000820 |         0 |     100 |      random tail |
|   timsort |   100000 |   32 | 0.002001 | 0.002022 |         0 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003638 | 0.003668 |         0 |     100 |      random half |
|  quadsort |   100000 |   32 | 0.001452 | 0.001465 |         0 |     100 |      random half |
|   timsort |   100000 |   32 | 0.004035 | 0.004056 |         0 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001104 | 0.001134 |         0 |     100 |  ascending tiles |
|  quadsort |   100000 |   32 | 0.000819 | 0.000829 |         0 |     100 |  ascending tiles |
|   timsort |   100000 |   32 | 0.000874 | 0.000980 |         0 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001779 | 0.001885 |         0 |     100 |     bit reversal |
|  quadsort |   100000 |   32 | 0.002238 | 0.002249 |         0 |     100 |     bit reversal |
|   timsort |   100000 |   32 | 0.002289 | 0.003090 |         0 |     100 |     bit reversal |

</details>

The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. It measures the performance on random data with array sizes
ranging from 1 to 1024. It's generated by running the benchmark using 1024 0 0 as the argument. The benchmark is weighted, meaning the number of repetitions
halves each time the number of items doubles. A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph2.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |        4 |   32 | 0.005569 | 0.005899 |       0.0 |      50 |       random 1-4 |
|  quadsort |        4 |   32 | 0.001144 | 0.001189 |       0.0 |      50 |       random 1-4 |
|   timsort |        4 |   32 | 0.002301 | 0.002491 |       0.0 |      50 |       random 1-4 |
|           |          |      |          |          |           |         |                  |
|stablesort |        8 |   32 | 0.005731 | 0.005950 |       0.0 |      50 |       random 5-8 |
|  quadsort |        8 |   32 | 0.002064 | 0.002200 |       0.0 |      50 |       random 5-8 |
|   timsort |        8 |   32 | 0.004958 | 0.005165 |       0.0 |      50 |       random 5-8 |
|           |          |      |          |          |           |         |                  |
|stablesort |       16 |   32 | 0.006360 | 0.006415 |       0.0 |      50 |      random 9-16 |
|  quadsort |       16 |   32 | 0.001862 | 0.001927 |       0.0 |      50 |      random 9-16 |
|   timsort |       16 |   32 | 0.006578 | 0.006663 |       0.0 |      50 |      random 9-16 |
|           |          |      |          |          |           |         |                  |
|stablesort |       32 |   32 | 0.007809 | 0.007885 |       0.0 |      50 |     random 17-32 |
|  quadsort |       32 |   32 | 0.003177 | 0.003258 |       0.0 |      50 |     random 17-32 |
|   timsort |       32 |   32 | 0.008597 | 0.008698 |       0.0 |      50 |     random 17-32 |
|           |          |      |          |          |           |         |                  |
|stablesort |       64 |   32 | 0.008846 | 0.008918 |       0.0 |      50 |     random 33-64 |
|  quadsort |       64 |   32 | 0.004144 | 0.004195 |       0.0 |      50 |     random 33-64 |
|   timsort |       64 |   32 | 0.011459 | 0.011560 |       0.0 |      50 |     random 33-64 |
|           |          |      |          |          |           |         |                  |
|stablesort |      128 |   32 | 0.010065 | 0.010131 |       0.0 |      50 |    random 65-128 |
|  quadsort |      128 |   32 | 0.005131 | 0.005184 |       0.0 |      50 |    random 65-128 |
|   timsort |      128 |   32 | 0.013917 | 0.014022 |       0.0 |      50 |    random 65-128 |
|           |          |      |          |          |           |         |                  |
|stablesort |      256 |   32 | 0.011217 | 0.011305 |       0.0 |      50 |   random 129-256 |
|  quadsort |      256 |   32 | 0.004937 | 0.005010 |       0.0 |      50 |   random 129-256 |
|   timsort |      256 |   32 | 0.015785 | 0.015912 |       0.0 |      50 |   random 129-256 |
|           |          |      |          |          |           |         |                  |
|stablesort |      512 |   32 | 0.012544 | 0.012637 |       0.0 |      50 |   random 257-512 |
|  quadsort |      512 |   32 | 0.005545 | 0.005618 |       0.0 |      50 |   random 257-512 |
|   timsort |      512 |   32 | 0.017533 | 0.017652 |       0.0 |      50 |   random 257-512 |
|           |          |      |          |          |           |         |                  |
|stablesort |     1024 |   32 | 0.013871 | 0.013979 |       0.0 |      50 |  random 513-1024 |
|  quadsort |     1024 |   32 | 0.005664 | 0.005755 |       0.0 |      50 |  random 513-1024 |
|   timsort |     1024 |   32 | 0.019176 | 0.019270 |       0.0 |      50 |  random 513-1024 |
|           |          |      |          |          |           |         |                  |
|stablesort |     2048 |   32 | 0.010961 | 0.011018 |       0.0 |      50 | random 1025-2048 |
|  quadsort |     2048 |   32 | 0.004527 | 0.004580 |       0.0 |      50 | random 1025-2048 |
|   timsort |     2048 |   32 | 0.015289 | 0.015338 |       0.0 |      50 | random 1025-2048 |
|           |          |      |          |          |           |         |                  |
|stablesort |     4096 |   32 | 0.010854 | 0.010917 |       0.0 |      50 | random 2049-4096 |
|  quadsort |     4096 |   32 | 0.003974 | 0.004018 |       0.0 |      50 | random 2049-4096 |
|   timsort |     4096 |   32 | 0.015051 | 0.015132 |       0.0 |      50 | random 2049-4096 |

</details>

Benchmark: quadsort vs qsort (mergesort)
----------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using gcc -O3 bench.c. Each test was ran 100 times. It's generated
by running the benchmark using 100000 100 1 as the argument. In the benchmark quadsort is
compared against glibc qsort() using the same general purpose interface and without any known
unfair advantage, like inlining. A table with the best and average time in seconds can be
uncollapsed below the bar graph.

![Graph](/images/graph3.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   64 | 0.016881 | 0.017052 |   1536381 |     100 |    random string |
|  quadsort |   100000 |   64 | 0.010615 | 0.010756 |   1655772 |     100 |    random string |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   64 | 0.015387 | 0.015550 |   1536491 |     100 |    random double |
|  quadsort |   100000 |   64 | 0.008648 | 0.008751 |   1655904 |     100 |    random double |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   64 | 0.011165 | 0.011375 |   1536491 |     100 |      random long |
|  quadsort |   100000 |   64 | 0.006024 | 0.006099 |   1655904 |     100 |      random long |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   64 | 0.010775 | 0.010928 |   1536634 |     100 |       random int |
|  quadsort |   100000 |   64 | 0.005313 | 0.005375 |   1655948 |     100 |       random int |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |  128 | 0.018214 | 0.018843 |   1536491 |     100 |     random order |
|  quadsort |   100000 |  128 | 0.011098 | 0.011185 |   1655904 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   64 | 0.009522 | 0.009748 |   1536491 |     100 |     random order |
|  quadsort |   100000 |   64 | 0.004073 | 0.004118 |   1655904 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   32 | 0.008946 | 0.009149 |   1536634 |     100 |     random order |
|  quadsort |   100000 |   32 | 0.003342 | 0.003391 |   1655948 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.006868 | 0.007059 |   1532324 |     100 |     random % 100 |
|  quadsort |   100000 |   32 | 0.002690 | 0.002740 |   1381730 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002612 | 0.002845 |    815024 |     100 |  ascending order |
|  quadsort |   100000 |   32 | 0.000160 | 0.000161 |     99999 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003396 | 0.003622 |    915020 |     100 |    ascending saw |
|  quadsort |   100000 |   32 | 0.000904 | 0.000925 |    368457 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002672 | 0.002803 |    884462 |     100 |       pipe organ |
|  quadsort |   100000 |   32 | 0.000466 | 0.000469 |    277443 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002469 | 0.002587 |    853904 |     100 | descending order |
|  quadsort |   100000 |   32 | 0.000164 | 0.000165 |     99999 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003302 | 0.003453 |    953892 |     100 |   descending saw |
|  quadsort |   100000 |   32 | 0.000929 | 0.000941 |    380548 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.004250 | 0.004501 |   1012003 |     100 |      random tail |
|  quadsort |   100000 |   32 | 0.001188 | 0.001208 |    564953 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005960 | 0.006133 |   1200707 |     100 |      random half |
|  quadsort |   100000 |   32 | 0.002047 | 0.002078 |    980778 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003903 | 0.004352 |   1209200 |     100 |  ascending tiles |
|  quadsort |   100000 |   32 | 0.002072 | 0.002170 |    671191 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005165 | 0.006168 |   1553378 |     100 |     bit reversal |
|  quadsort |   100000 |   32 | 0.003146 | 0.003197 |   1711215 |     100 |     bit reversal |

</details>

The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using gcc -O3 bench.c. Each test was ran 100 times. It's generated by running the benchmark using
10000000 0 0 as the argument. The benchmark is weighted, meaning the number of repetitions
halves each time the number of items doubles.  A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph5.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |       10 |   32 | 0.218310 | 0.224505 |        22 |      10 |        random 10 |
|  quadsort |       10 |   32 | 0.091750 | 0.092312 |        29 |      10 |        random 10 |
|           |          |      |          |          |           |         |                  |
|     qsort |      100 |   32 | 0.391962 | 0.396639 |       541 |      10 |       random 100 |
|  quadsort |      100 |   32 | 0.173928 | 0.177794 |       646 |      10 |       random 100 |
|           |          |      |          |          |           |         |                  |
|     qsort |     1000 |   32 | 0.558055 | 0.566364 |      8707 |      10 |      random 1000 |
|  quadsort |     1000 |   32 | 0.220395 | 0.222146 |      9817 |      10 |      random 1000 |
|           |          |      |          |          |           |         |                  |
|     qsort |    10000 |   32 | 0.735528 | 0.741353 |    120454 |      10 |     random 10000 |
|  quadsort |    10000 |   32 | 0.267860 | 0.269924 |    131668 |      10 |     random 10000 |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.907161 | 0.910446 |   1536421 |      10 |    random 100000 |
|  quadsort |   100000 |   32 | 0.339541 | 0.340942 |   1655703 |      10 |    random 100000 |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 1.085275 | 1.089068 |  18674532 |      10 |   random 1000000 |
|  quadsort |  1000000 |   32 | 0.401715 | 0.403860 |  19816270 |      10 |   random 1000000 |
|           |          |      |          |          |           |         |                  |
|     qsort | 10000000 |   32 | 1.313922 | 1.319911 | 220105921 |      10 |  random 10000000 |
|  quadsort | 10000000 |   32 | 0.599393 | 0.601635 | 231513131 |      10 |  random 10000000 |

</details>

Benchmark: quadsort vs pdqsort (pattern defeating quicksort)
------------------------------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. Pdqsort is a branchless
quicksort/insertionsort hybrid. [Fluxsort](https://github.com/scandum/fluxsort) is a branchless quicksert/mergesort hybrid. Each test
was ran 100 times on 100,000 elements. Comparisons are fully inlined. A table with the best and
average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph4.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|   pdqsort |   100000 |  128 | 0.005773 | 0.005859 |         0 |     100 |     random order |
|  quadsort |   100000 |  128 | 0.009813 | 0.009882 |         0 |     100 |     random order |
|  fluxsort |   100000 |  128 | 0.008603 | 0.008704 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|   pdqsort |   100000 |   64 | 0.002671 | 0.002686 |         0 |     100 |     random order |
|  quadsort |   100000 |   64 | 0.002516 | 0.002534 |         0 |     100 |     random order |
|  fluxsort |   100000 |   64 | 0.001978 | 0.002003 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|   pdqsort |   100000 |   32 | 0.002589 | 0.002607 |         0 |     100 |     random order |
|  quadsort |   100000 |   32 | 0.002447 | 0.002466 |         0 |     100 |     random order |
|  fluxsort |   100000 |   32 | 0.001851 | 0.001873 |         0 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.000780 | 0.000788 |         0 |     100 |     random % 100 |
|  quadsort |   100000 |   32 | 0.001788 | 0.001812 |         0 |     100 |     random % 100 |
|  fluxsort |   100000 |   32 | 0.000675 | 0.000688 |         0 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.000084 | 0.000085 |         0 |     100 |  ascending order |
|  quadsort |   100000 |   32 | 0.000051 | 0.000051 |         0 |     100 |  ascending order |
|  fluxsort |   100000 |   32 | 0.000042 | 0.000043 |         0 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.003378 | 0.003402 |         0 |     100 |    ascending saw |
|  quadsort |   100000 |   32 | 0.000615 | 0.000618 |         0 |     100 |    ascending saw |
|  fluxsort |   100000 |   32 | 0.000327 | 0.000337 |         0 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002772 | 0.002792 |         0 |     100 |       pipe organ |
|  quadsort |   100000 |   32 | 0.000271 | 0.000271 |         0 |     100 |       pipe organ |
|  fluxsort |   100000 |   32 | 0.000214 | 0.000215 |         0 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.000187 | 0.000192 |         0 |     100 | descending order |
|  quadsort |   100000 |   32 | 0.000059 | 0.000059 |         0 |     100 | descending order |
|  fluxsort |   100000 |   32 | 0.000053 | 0.000053 |         0 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.003148 | 0.003165 |         0 |     100 |   descending saw |
|  quadsort |   100000 |   32 | 0.000614 | 0.000626 |         0 |     100 |   descending saw |
|  fluxsort |   100000 |   32 | 0.000327 | 0.000331 |         0 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002498 | 0.002520 |         0 |     100 |      random tail |
|  quadsort |   100000 |   32 | 0.000813 | 0.000842 |         0 |     100 |      random tail |
|  fluxsort |   100000 |   32 | 0.000624 | 0.000627 |         0 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002573 | 0.002590 |         0 |     100 |      random half |
|  quadsort |   100000 |   32 | 0.001451 | 0.001462 |         0 |     100 |      random half |
|  fluxsort |   100000 |   32 | 0.001064 | 0.001075 |         0 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002256 | 0.002281 |         0 |     100 |  ascending tiles |
|  quadsort |   100000 |   32 | 0.000815 | 0.000823 |         0 |     100 |  ascending tiles |
|  fluxsort |   100000 |   32 | 0.000313 | 0.000315 |         0 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002570 | 0.002589 |         0 |     100 |     bit reversal |
|  quadsort |   100000 |   32 | 0.002230 | 0.002259 |         0 |     100 |     bit reversal |
|  fluxsort |   100000 |   32 | 0.001718 | 0.001744 |         0 |     100 |     bit reversal |

</details>
