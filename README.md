Intro
-----

This document describes a stable bottom-up adaptive branchless merge sort named quadsort. A [visualisation](https://github.com/scandum/quadsort#visualization) and [benchmarks](https://github.com/scandum/quadsort#benchmark-quadsort-vs-stdstable_sort-vs-timsort) are available at the bottom.

The quad swap
-------------

At the core of quadsort is the quad swap. Traditionally most sorting
algorithms have been designed using the binary swap where two variables
are sorted using a third temporary variable. This typically looks as
following.
```c
    if (val[0] > val[1])
    {
        swap[0] = val[0];
        val[0] = val[1];
        val[1] = swap[0];
    }
```
Instead the quad swap sorts four variables at once. During the first
stage the four variables are partially sorted in the four
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
swap variables are sorted in-order, if that's the case the swap
finishes up immediately. Next it checks if the swap variables
are sorted in reverse-order, if that's the case the sort finishes up
immediately. If both checks fail the final arrangement is known and
two checks remain to determine the final order.

This eliminates 1 wasteful comparison for in-order sequences while creating
1 additional comparison for random sequences. However, in the real world we
are rarely comparing truly random data, so in any instance where data is
more likely to be orderly than disorderly this shift in probability will give
an advantage.

In C the basic quad swap looks as following:
```c
    if (val[0] > val[1])
    {
        swap[0] = val[1];
        swap[1] = val[0];
    }
    else
    {
        swap[0] = val[0];
        swap[1] = val[1];
    }

    if (val[2] > val[3])
    {
        swap[2] = val[3];
        swap[3] = val[2];
    }
    else
    {
        swap[2] = val[2];
        swap[3] = val[3];
    }

    if (swap[1] <= swap[2])
    {
        val[0] = swap[0];
        val[1] = swap[1];
        val[2] = swap[2];
        val[3] = swap[3];
    }
    else if (swap[0] > swap[3])
    {
        val[0] = swap[2];
        val[1] = swap[3];
        val[2] = swap[0];
        val[3] = swap[1];
    }
    else
    {
       if (swap[0] <= swap[2])
       {
           val[0] = swap[0];
           val[1] = swap[2];
       }
       else
       {
           val[0] = swap[2];
           val[1] = swap[0];
       }

       if (swap[1] <= swap[3])
       {
           val[2] = swap[1];
           val[3] = swap[3];
       }
       else
       {
           val[2] = swap[3];
           val[3] = swap[1];
       }
    }
```

In-place quad swap
------------------
There are however several problems with the simple quad swap above. If an array is already fully sorted it writes a lot of data back and forth from swap unnecessarily. If an array is fully in reverse order it will change **8 7 6 5  4 3 2 1** to **5 6 7 8  1 2 3 4** which reduces the degree of orderliness rather than increasing it.

To solve these problems the quad swap needs to be implemented in-place.

Reverse order handling
----------------------
Reverse order data is typically handled using a simple reversal function, as following.
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
and **n** moves through run detection. Without run detection the best you
can do is sort it in **n** comparisons and **n log n** moves. 

Run detection, as the name implies, comes with a detection cost. Thanks
to the laws of probability a quad swap can cheat however. The chance of
4 random numbers having the order **4 3 2 1** is 1 in 24. So when sorting
random data we'll only make a wasteful run check in 4.16% of cases.

What about run detection for in-order data? While we're turning
**n log n** moves into **n** moves with reverse order run detection, we'd be
turning **0** moves into **0** moves with forward run detection. So there's
no point in doing so.

The next optimization is to write the quad swap in such a way that we can
perform a simple check to see if the entire array was in reverse order,
if so, the sort is finished.

One final optimization, reverse order handling is only beneficial on
runs longer than 4 elements. When no reverse order run is detected
the next 4 elements are merged with the first 4 elements. This reduces
the chance of a wasteful run check to 2.08%.

At the end of the loop the array has been turned into a series of ordered
blocks of 8 elements.

Ping-Pong Merge
---------------
Traditionally mergesorts would merge two blocks to swap memory, then copy
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
Overall the quad galloping merge gives a slight performance gain for both ordered and random data.

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
space of quadsort to **n / 2** and that the quad merge strategy works best
on arrays of different lengths. It also greatly simplifies the ping-pong
merge routine which only needs to work on arrays of equal length.

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

Memory
------
By default quadsort uses between n and n / 4 swap memory. If memory allocation fails quadsort will switch to sorting in-place through rotations. The minimum memory requirement is 32 elements of stack memory.

Performance
-----------
Quadsort is one of the fastest merge sorts written to date. It is faster than quicksort for most data distributions, with the notable exception of generic data.

On arrays exceeding the L1 cache quicksort has an advantage due to its ability to partition. For small arrays quadsort has a significant advantage due to quicksort's inability to cost effectively pick a reliable pivot.

To take full advantage of branchless operations the cmp macro needs to be uncommented in bench.c, which will increase the performance by 30% on primitive types.

Variants
--------
- [blitsort](https://github.com/scandum/blitsort) is a hybrid stable in-place rotate quicksort / quadsort.

- [crumsort](https://github.com/scandum/crumsort) is a hybrid unstable in-place quicksort / quadsort.

- [fluxsort](https://github.com/scandum/fluxsort) is a hybrid stable quicksort / quadsort.

- [gridsort](https://github.com/scandum/gridsort) is a hybrid stable cubesort / quadsort. Gridsort makes O(n) moves rather than the typical O(n log n) moves. It is an online sort and might be of interest to those interested in data structures and sorting very large arrays.

- [twinsort](https://github.com/scandum/twinsort) is a simplified quadsort with a
much smaller code size. Twinsort might be of use to people who want to port or understand quadsort; it does not use
pointers or gotos.

- [wolfsort](https://github.com/scandum/wolfsort) is a hybrid stable radixsort / fluxsort with improved performance on random data. It's mostly a proof of concept that only works on unsigned 32 bit integers.

- [Robin Hood Sort](https://github.com/mlochbaum/rhsort) is a hybrid stable radixsort / dropsort with improved performance on random and generic data. It has a compilation option to use quadsort for its merging.

Credits
-------
I personally invented the quad swap, quad galloping merge, parity merge, branchless parity merge,
monobound binary search, bridge rotation, and trinity rotation.

The ping-pong merge had been independently implemented in wikisort prior to quadsort, and
likely by others as well.

The monobound binary search has been independently implemented, often referred to as a branchless binary search.

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

|      Name |     Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | --------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |    100000 |   32 | 0.006096 | 0.006141 |         1 |     100 |     random order |
|  quadsort |    100000 |   32 | 0.002677 | 0.002696 |         1 |     100 |     random order |
|   timsort |    100000 |   32 | 0.007388 | 0.007431 |         1 |     100 |     random order |
|           |           |      |          |          |           |         |                  |
|stablesort |    100000 |   32 | 0.003936 | 0.003974 |         1 |     100 |     random % 100 |
|  quadsort |    100000 |   32 | 0.001971 | 0.001983 |         1 |     100 |     random % 100 |
|   timsort |    100000 |   32 | 0.005349 | 0.005387 |         1 |     100 |     random % 100 |
|           |           |      |          |          |           |         |                  |
|stablesort |    100000 |   32 | 0.000692 | 0.000711 |         1 |     100 |  ascending order |
|  quadsort |    100000 |   32 | 0.000069 | 0.000069 |         1 |     100 |  ascending order |
|   timsort |    100000 |   32 | 0.000045 | 0.000045 |         1 |     100 |  ascending order |
|           |           |      |          |          |           |         |                  |
|stablesort |    100000 |   32 | 0.001378 | 0.001409 |         1 |     100 |    ascending saw |
|  quadsort |    100000 |   32 | 0.000639 | 0.000648 |         1 |     100 |    ascending saw |
|   timsort |    100000 |   32 | 0.000834 | 0.000842 |         1 |     100 |    ascending saw |
|           |           |      |          |          |           |         |                  |
|stablesort |    100000 |   32 | 0.000838 | 0.000856 |         1 |     100 |       pipe organ |
|  quadsort |    100000 |   32 | 0.000266 | 0.000269 |         1 |     100 |       pipe organ |
|   timsort |    100000 |   32 | 0.000175 | 0.000176 |         1 |     100 |       pipe organ |
|           |           |      |          |          |           |         |                  |
|stablesort |    100000 |   32 | 0.000926 | 0.000938 |         1 |     100 | descending order |
|  quadsort |    100000 |   32 | 0.000056 | 0.000056 |         1 |     100 | descending order |
|   timsort |    100000 |   32 | 0.000101 | 0.000101 |         1 |     100 | descending order |
|           |           |      |          |          |           |         |                  |
|stablesort |    100000 |   32 | 0.001377 | 0.001408 |         1 |     100 |   descending saw |
|  quadsort |    100000 |   32 | 0.000639 | 0.000645 |         1 |     100 |   descending saw |
|   timsort |    100000 |   32 | 0.000835 | 0.000853 |         1 |     100 |   descending saw |
|           |           |      |          |          |           |         |                  |
|stablesort |    100000 |   32 | 0.002074 | 0.002094 |         1 |     100 |      random tail |
|  quadsort |    100000 |   32 | 0.000882 | 0.000888 |         1 |     100 |      random tail |
|   timsort |    100000 |   32 | 0.001945 | 0.001957 |         1 |     100 |      random tail |
|           |           |      |          |          |           |         |                  |
|stablesort |    100000 |   32 | 0.003553 | 0.003578 |         1 |     100 |      random half |
|  quadsort |    100000 |   32 | 0.001576 | 0.001585 |         1 |     100 |      random half |
|   timsort |    100000 |   32 | 0.003921 | 0.003947 |         1 |     100 |      random half |
|           |           |      |          |          |           |         |                  |
|stablesort |    100000 |   32 | 0.000982 | 0.001006 |         1 |     100 |  ascending tiles |
|  quadsort |    100000 |   32 | 0.000812 | 0.000823 |         1 |     100 |  ascending tiles |
|   timsort |    100000 |   32 | 0.000879 | 0.000916 |         1 |     100 |  ascending tiles |
|           |           |      |          |          |           |         |                  |
|stablesort |    100000 |   32 | 0.001532 | 0.001973 |         1 |     100 |     bit reversal |
|  quadsort |    100000 |   32 | 0.002336 | 0.002347 |         1 |     100 |     bit reversal |
|   timsort |    100000 |   32 | 0.002197 | 0.002762 |         1 |     100 |     bit reversal |

</details>

The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. It measures the performance on random data with array sizes
ranging from 8 to 524288. The benchmark is weighted, meaning the number of repetitions
halves each time the number of items doubles. A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph2.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |      Reps | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |        8 |   32 | 0.006182 | 0.006221 |     65536 |     100 |         random 8 |
|  quadsort |        8 |   32 | 0.001628 | 0.001644 |     65536 |     100 |         random 8 |
|   timsort |        8 |   32 | 0.006378 | 0.006680 |     65536 |     100 |         random 8 |
|           |          |      |          |          |           |         |                  |
|stablesort |       32 |   32 | 0.009514 | 0.009653 |     16384 |     100 |        random 32 |
|  quadsort |       32 |   32 | 0.003978 | 0.004041 |     16384 |     100 |        random 32 |
|   timsort |       32 |   32 | 0.013349 | 0.013524 |     16384 |     100 |        random 32 |
|           |          |      |          |          |           |         |                  |
|stablesort |      128 |   32 | 0.012961 | 0.013043 |      4096 |     100 |       random 128 |
|  quadsort |      128 |   32 | 0.005179 | 0.005270 |      4096 |     100 |       random 128 |
|   timsort |      128 |   32 | 0.019949 | 0.020191 |      4096 |     100 |       random 128 |
|           |          |      |          |          |           |         |                  |
|stablesort |      512 |   32 | 0.016774 | 0.016886 |      1024 |     100 |       random 512 |
|  quadsort |      512 |   32 | 0.006800 | 0.006895 |      1024 |     100 |       random 512 |
|   timsort |      512 |   32 | 0.024646 | 0.024794 |      1024 |     100 |       random 512 |
|           |          |      |          |          |           |         |                  |
|stablesort |     2048 |   32 | 0.020439 | 0.020530 |       256 |     100 |      random 2048 |
|  quadsort |     2048 |   32 | 0.008351 | 0.008394 |       256 |     100 |      random 2048 |
|   timsort |     2048 |   32 | 0.028911 | 0.029043 |       256 |     100 |      random 2048 |
|           |          |      |          |          |           |         |                  |
|stablesort |     8192 |   32 | 0.024204 | 0.024298 |        64 |     100 |      random 8192 |
|  quadsort |     8192 |   32 | 0.009967 | 0.010036 |        64 |     100 |      random 8192 |
|   timsort |     8192 |   32 | 0.033020 | 0.033134 |        64 |     100 |      random 8192 |
|           |          |      |          |          |           |         |                  |
|stablesort |    32768 |   32 | 0.028113 | 0.028206 |        16 |     100 |     random 32768 |
|  quadsort |    32768 |   32 | 0.011645 | 0.011692 |        16 |     100 |     random 32768 |
|   timsort |    32768 |   32 | 0.037227 | 0.037328 |        16 |     100 |     random 32768 |
|           |          |      |          |          |           |         |                  |
|stablesort |   131072 |   32 | 0.032113 | 0.032197 |         4 |     100 |    random 131072 |
|  quadsort |   131072 |   32 | 0.013344 | 0.013388 |         4 |     100 |    random 131072 |
|   timsort |   131072 |   32 | 0.041431 | 0.041528 |         4 |     100 |    random 131072 |
|           |          |      |          |          |           |         |                  |
|stablesort |   524288 |   32 | 0.036110 | 0.036244 |         1 |     100 |    random 524288 |
|  quadsort |   524288 |   32 | 0.015046 | 0.015115 |         1 |     100 |    random 524288 |
|   timsort |   524288 |   32 | 0.045705 | 0.045935 |         1 |     100 |    random 524288 |

</details>

Benchmark: quadsort vs qsort (mergesort)
----------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using gcc -O3 bench.c. Each test was ran 10 times. It's generated
by running the benchmark using 100000 100 1 as the argument. In the benchmark quadsort is
compared against glibc qsort() using the same general purpose interface and without any known
unfair advantage, like inlining. A table with the best and average time in seconds can be
uncollapsed below the bar graph.

![Graph](/images/graph3.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   64 | 0.017014 | 0.017276 |   1536228 |      10 |    random string |
|  quadsort |   100000 |   64 | 0.011095 | 0.011311 |   1650839 |      10 |    random string |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |  128 | 0.019056 | 0.019849 |   1536323 |      10 |     random order |
|  quadsort |   100000 |  128 | 0.012036 | 0.012106 |   1651422 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   64 | 0.009368 | 0.009600 |   1536323 |      10 |     random order |
|  quadsort |   100000 |   64 | 0.004311 | 0.004383 |   1651422 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   32 | 0.008917 | 0.009079 |   1536558 |      10 |     random order |
|  quadsort |   100000 |   32 | 0.003550 | 0.003554 |   1650950 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.006732 | 0.006882 |   1532595 |      10 |     random % 100 |
|  quadsort |   100000 |   32 | 0.002869 | 0.002874 |   1377639 |      10 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002248 | 0.002406 |    815024 |      10 |  ascending order |
|  quadsort |   100000 |   32 | 0.000200 | 0.000203 |     99999 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003086 | 0.003249 |    915016 |      10 |    ascending saw |
|  quadsort |   100000 |   32 | 0.000945 | 0.000960 |    368067 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002502 | 0.002591 |    884462 |      10 |       pipe organ |
|  quadsort |   100000 |   32 | 0.000481 | 0.000483 |    277410 |      10 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002462 | 0.002519 |    853904 |      10 | descending order |
|  quadsort |   100000 |   32 | 0.000165 | 0.000166 |     99999 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003308 | 0.003398 |    953901 |      10 |   descending saw |
|  quadsort |   100000 |   32 | 0.000946 | 0.000955 |    380124 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003960 | 0.004027 |   1012038 |      10 |      random tail |
|  quadsort |   100000 |   32 | 0.001278 | 0.001281 |    562760 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005834 | 0.005985 |   1200828 |      10 |      random half |
|  quadsort |   100000 |   32 | 0.002162 | 0.002176 |    974954 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.004132 | 0.004509 |   1209200 |      10 |  ascending tiles |
|  quadsort |   100000 |   32 | 0.002137 | 0.002224 |    658723 |      10 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005134 | 0.005557 |   1553378 |      10 |     bit reversal |
|  quadsort |   100000 |   32 | 0.003227 | 0.003254 |   1711215 |      10 |     bit reversal |

</details>

The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using gcc -O3 bench.c. Each test was ran 100 times. It's generated by running the benchmark using
1000000 0 0 as the argument. The benchmark is weighted, meaning the number of repetitions
halves each time the number of items doubles.  A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph5.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |        8 |   32 | 0.009393 | 0.010391 |        17 |     100 |         random 8 |
|  quadsort |        8 |   32 | 0.003489 | 0.003504 |        21 |     100 |         random 8 |
|           |          |      |          |          |           |         |                  |
|     qsort |       32 |   32 | 0.014392 | 0.015333 |       121 |     100 |        random 32 |
|  quadsort |       32 |   32 | 0.006165 | 0.006362 |       146 |     100 |        random 32 |
|           |          |      |          |          |           |         |                  |
|     qsort |      128 |   32 | 0.019507 | 0.020682 |       745 |     100 |       random 128 |
|  quadsort |      128 |   32 | 0.008249 | 0.008440 |       840 |     100 |       random 128 |
|           |          |      |          |          |           |         |                  |
|     qsort |      512 |   32 | 0.024801 | 0.025837 |      3968 |     100 |       random 512 |
|  quadsort |      512 |   32 | 0.009946 | 0.010153 |      4388 |     100 |       random 512 |
|           |          |      |          |          |           |         |                  |
|     qsort |     2048 |   32 | 0.029895 | 0.030677 |     19962 |     100 |      random 2048 |
|  quadsort |     2048 |   32 | 0.011792 | 0.011903 |     21641 |     100 |      random 2048 |
|           |          |      |          |          |           |         |                  |
|     qsort |     8192 |   32 | 0.035080 | 0.036282 |     96149 |     100 |      random 8192 |
|  quadsort |     8192 |   32 | 0.013840 | 0.014034 |    102965 |     100 |      random 8192 |
|           |          |      |          |          |           |         |                  |
|     qsort |    32768 |   32 | 0.040274 | 0.041420 |    450105 |     100 |     random 32768 |
|  quadsort |    32768 |   32 | 0.015838 | 0.016109 |    477571 |     100 |     random 32768 |
|           |          |      |          |          |           |         |                  |
|     qsort |   131072 |   32 | 0.045464 | 0.047044 |   2062601 |     100 |    random 131072 |
|  quadsort |   131072 |   32 | 0.017921 | 0.018207 |   2172670 |     100 |    random 131072 |
|           |          |      |          |          |           |         |                  |
|     qsort |   524288 |   32 | 0.050499 | 0.051669 |   9298689 |     100 |    random 524288 |
|  quadsort |   524288 |   32 | 0.019855 | 0.020284 |   9739167 |     100 |    random 524288 |

</details>

Benchmark: quadsort vs pdqsort (pattern defeating quicksort)
------------------------------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. Pdqsort is a branchless
quicksort/insertionsort hybrid. Each test was ran 100 times on 100,000 elements. Comparisons
are fully inlined. A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph4.png)

<details><summary><b>data table</b></summary>

|      Name |     Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | --------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|   pdqsort |    100000 |   32 | 0.002679 | 0.002701 |         1 |     100 |     random order |
|  quadsort |    100000 |   32 | 0.002675 | 0.002692 |         1 |     100 |     random order |
|           |           |      |          |          |           |         |                  |
|   pdqsort |    100000 |   32 | 0.000777 | 0.000786 |         1 |     100 |     random % 100 |
|  quadsort |    100000 |   32 | 0.001969 | 0.001988 |         1 |     100 |     random % 100 |
|           |           |      |          |          |           |         |                  |
|   pdqsort |    100000 |   32 | 0.000085 | 0.000085 |         1 |     100 |  ascending order |
|  quadsort |    100000 |   32 | 0.000069 | 0.000069 |         1 |     100 |  ascending order |
|           |           |      |          |          |           |         |                  |
|   pdqsort |    100000 |   32 | 0.003489 | 0.003509 |         1 |     100 |    ascending saw |
|  quadsort |    100000 |   32 | 0.000634 | 0.000640 |         1 |     100 |    ascending saw |
|           |           |      |          |          |           |         |                  |
|   pdqsort |    100000 |   32 | 0.002828 | 0.002853 |         1 |     100 |       pipe organ |
|  quadsort |    100000 |   32 | 0.000265 | 0.000267 |         1 |     100 |       pipe organ |
|           |           |      |          |          |           |         |                  |
|   pdqsort |    100000 |   32 | 0.000188 | 0.000189 |         1 |     100 | descending order |
|  quadsort |    100000 |   32 | 0.000056 | 0.000057 |         1 |     100 | descending order |
|           |           |      |          |          |           |         |                  |
|   pdqsort |    100000 |   32 | 0.003148 | 0.003166 |         1 |     100 |   descending saw |
|  quadsort |    100000 |   32 | 0.000636 | 0.000651 |         1 |     100 |   descending saw |
|           |           |      |          |          |           |         |                  |
|   pdqsort |    100000 |   32 | 0.002564 | 0.002578 |         1 |     100 |      random tail |
|  quadsort |    100000 |   32 | 0.000880 | 0.000888 |         1 |     100 |      random tail |
|           |           |      |          |          |           |         |                  |
|   pdqsort |    100000 |   32 | 0.002638 | 0.002653 |         1 |     100 |      random half |
|  quadsort |    100000 |   32 | 0.001573 | 0.001582 |         1 |     100 |      random half |
|           |           |      |          |          |           |         |                  |
|   pdqsort |    100000 |   32 | 0.002310 | 0.002335 |         1 |     100 |  ascending tiles |
|  quadsort |    100000 |   32 | 0.000819 | 0.000830 |         1 |     100 |  ascending tiles |
|           |           |      |          |          |           |         |                  |
|   pdqsort |    100000 |   32 | 0.002660 | 0.002679 |         1 |     100 |     bit reversal |
|  quadsort |    100000 |   32 | 0.002338 | 0.002357 |         1 |     100 |     bit reversal |

</details>
