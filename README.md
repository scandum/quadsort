Intro
-----

This document describes a stable bottom-up adaptive merge sort named quadsort.


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

There is also an overall performance increase due to the elimination of
wasteful swapping. In C the basic quad swap looks as following:
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
In the case the array cannot be perfectly divided by 4, the tail, existing
of 1-3 elements, is sorted using the traditional swap.

In-place quad swap
------------------
There are however several problems with the simple quad swap above. If an array is already fully sorted it writes a lot of data back and forth from swap unnecessarily. If an array is fully in reverse order it will change **8 7 6 5  4 3 2 1** to **5 6 7 8  1 2 3 4** which reduces the degree of orderliness rather than increasing it.

To solve these problems the quad swap needs to be implemented in-place.

Chain swap
----------
The chain swap is easiest explained with an example. Traditionally many sorts would sort three random values by executing three binary swaps.
```c
int swap_two(int a, int b, int swap)
{
    swap = a; a = b; b = swap;
}

int swap_three(int array[], swap)
{
    swap_two(array[0], array[1], swap);
    swap_two(array[1], array[2], swap);
    swap_two(array[0], array[1], swap);
}
```
While placing the swap operation `swap = a;a = b;b = swap;` on one line might be confusing, it does illustrate the symmetric nature of the assignment better than placing it on three lines.

Swapping like this, while convenient, is obviously not the most efficient route to take. So an in-place quadswap implements the sorting of three values as following.
```c
int swap_three(int array[], swap)
{
    if (array[0] > array[1])
    {
        if (array[0] <= array[2])
        {
            swap = array[0]; array[0] = array[1]; array[1] = swap;
        }
        else if (array[1] > array[2])
        {
            swap = array[0]; array[0] = array[2]; array[2] = swap;
        }
        else
        {
            swap = array[0]; array[0] = array[1]; array[1] = array[2]; array[2] = swap;
        }
    }
    else if (array[1] > array[2])
    {
        if (array[0] > array[2])
        {
            swap = array[2]; array[2] = array[1]; array[1] = array[0]; array[0] = swap;
        }
        else
        {
            swap = array[2]; array[2] = array[1]; array[1] = swap;
        }
    }
}
```
While swapping like this takes up a lot more real estate the advantages should be pretty clear. By doing a triple swap you always perform 3 comparisons and up to 3 swaps. By conjoining the three operations you perform only 2 comparisons in the best case and the swaps are chained together turning a worst case of 9 assignments into a worst case of 4.

If the array is already in-order no assignments take place.

Reverse order handling
----------------------
As mentioned previously, reverse order data has a high degree of orderliness and subsequently it can be sorted efficiently. In fact, if a quad swap were to turn **9 8 7 6  5 4 3 2  1** into **6 7 8 9  2 3 4 5  1** it would be taking a step backward instead of forward. Reverse order data is typically handled using a simple reversal function, as following.
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
random blocks of 4 elements, by expanding the sorting network, a quad
swap only has to check if it's dealing with a reverse-order run when it
encounters a reverse order sequence (like **4 3 2 1**), which for random
data occurs in 4.16% of cases.

What about run detection for in-order data? While we're turning
**n log n** moves into **n** moves with reverse order run detection, we'd be
turning **0** moves into **0** moves with forward run detection. There would
still be the advantage of only having to check in-order runs in 4.16% of
cases. However, the benefit of turning **n log n** moves into **0** moves 
is so massive that we want to check for in-order runs in 100% of cases.

But doing in-order run checks in the quad swap routine is not efficient
because that would mean we need to start remembering run lengths and
perform other kinds of algorithmic gymnastics. Instead we keep it simple
and check in-order runs at a later stage.

One last optimization is to write the quad swap in such a way that we can
perform a simple check to see if the entire array was in reverse order,
if so, the sort is finished. If not, we know the array has been turned
into a series of ordered blocks of 4 elements.

Quad merge
----------

In the first stage of quadsort the quad swap is used to pre-sort the
array into sorted 4-element blocks as described above.

The second stage uses an approach similar to the quad swap, but it's
sorting blocks of 4, 16, 64, or more elements.

The quad merge can be visualized as following:
```
    main memory:  [A][B][C][D]
    swap memory:  [A  B]        step 1
    swap memory:  [A  B][C  D]  step 2
    main memory:  [A  B  C  D]  step 3
```

In the first row quad swap has been used to create 4 blocks of 4 sorted
elements each. In the second row, step 1, block A and B have been merged
to swap memory into a single sorted block of 8 elements. In the third row,
step 2, block C and D have also been merged to swap memory. In the last row,
step 3, the blocks are merged back to main memory and we're left with 1 block
of 16 sorted elements. The following is a visualization of an array with 64
random elements getting sorted.

![quadsort visualization](/images/quadsort.gif)

Skipping
--------

Just like with the quad swap it is beneficial to check whether the 4 blocks
are in-order.

In the case of the 4 blocks being in-order the merge operation is skipped,
as this would be pointless. This does however require an extra if check, and
for randomly sorted data this if check becomes increasingly unlikely to be
true as the block size increases. Fortunately the frequency of this if check
is quartered each loop, while the potential benefit is quadrupled each loop.

Because reverse order data is handled in the quad swap there is no need to
check for reverse order blocks.

In the case only 2 out of 4 blocks are in-order the comparisons in the merge
itself are unnecessary and subsequently omitted. The data still needs to be
copied to swap memory.

This allows quadsort to sort in-order sequences using **n** comparisons instead
of **n * log n** comparisons.

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
This unguarded merge optimization is most effective in the final tail merge.

tail merge
----------
When sorting an array of 65 elements you end up with a sorted array of 64
elements and a sorted array of 1 element in the end. If a program sorts in
intervals it should pick an optimal array size (64, 256, 1024, etc) to do so.

Another problem is that a sub-optimal array size results in wasteful swapping. To
work around these two problems the quad merge routine is aborted when the
block size reaches 1/8th of the array size, and the remainder of the array
is sorted using a tail merge.

The main advantage of the tail merge is that it allows reducing the swap
space of quadsort to **n / 2** and that it has been optimized to merge arrays
of different lengths. It also simplifies the quad merge routine which only
needs to work on arrays of equal length.

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
│quicksort      ││n      │n log n│n²     ││1      │1      │1      ││no    ││yes      ││no       │
└───────────────┘└───────┴───────┴───────┘└───────┴───────┴───────┘└──────┘└─────────┘└─────────┘
```
Quadsort makes n comparisons when the data is already sorted or reverse sorted.

Data Types
----------
The C implementation of quadsort supports long doubles and 8, 16, 32, and 64 bit data types. By using pointers it's possible to sort any other data type.

Interface
---------
Quadsort uses the same interface as qsort, which is described in [man qsort](https://man7.org/linux/man-pages/man3/qsort.3p.html).

Cache
-----
Because quadsort uses n / 2 swap memory and does not partition its cache utilization
is not as ideal as quicksort. Based on my benchmarks it appears that quadsort is faster than
in-place sorts for array sizes that do not exhaust the L1 cache, which can be up
to 64KB on modern processors.

While quadsort is faster than quicksort it will be slower than a well written hybrid quicksort on
larger random distributions. It will beat hybrid quicksorts on ordered distributions.

Variants
--------
- [gridsort](https://github.com/scandum/gridsort) is a hybrid cubesort / quadsort with improved performance on random data.

- [twinsort](https://github.com/scandum/twinsort) is a simplified quadsort with a
much smaller code size. Twinsort might be of use to people who want to port or understand quadsort; it does not use
pointers or gotos.

- [wolfsort](https://github.com/scandum/wolfsort) is a hybrid radixsort / quadsort with improved performance on random data. It's mostly a proof of concept that only work on unsigned 32 and 64 bit integers.

- [fluxsort](https://github.com/scandum/wolfsort) is a hybrid partition / quadsort with improved performance on random data. It doesn't have its own project page and is just tossed in with the other sorts in the wolfsort benchmark.

- [octosort](https://github.com/scandum/octosort) is based on quadsort and [WikiSort](https://github.com/BonzaiThePenguin/WikiSort). It operates with O(1) memory while being only slightly slower.

Visualization
-------------

In the visualization below four tests are performed. The first test is on a random
distribution, the second on an ascending distribution, the third on a descending
distribution, and the fourth on an ascending distribution with a random tail.

The upper half shows the swap memory and the bottom half shows the main memory.
Colors are used to differentiate between skip, swap, merge, and copy operations.

![quadsort benchmark](/images/benchmark.gif)

Benchmark: quadsort vs std::stable_sort vs timsort vs pdqsort vs wolfsort
-------------------------------------------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. Each test was ran 100 times
and only the best run is reported.

![Graph](/images/graph1.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |   100000 |   32 | 0.005997 | 0.006029 |         1 |     100 |     random order |
|  gridsort |   100000 |   32 | 0.004571 | 0.004599 |         1 |     100 |     random order |
|   timsort |   100000 |   32 | 0.007613 | 0.007634 |         1 |     100 |     random order |
|  quadsort |   100000 |   32 | 0.005826 | 0.005851 |         1 |     100 |     random order |
|  fluxsort |   100000 |   32 | 0.003984 | 0.003995 |         1 |     100 |     random order |
|  wolfsort |   100000 |   32 | 0.001280 | 0.001300 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000684 | 0.000731 |         1 |     100 |  ascending order |
|  gridsort |   100000 |   32 | 0.000368 | 0.000378 |         1 |     100 |  ascending order |
|   timsort |   100000 |   32 | 0.000045 | 0.000046 |         1 |     100 |  ascending order |
|  quadsort |   100000 |   32 | 0.000064 | 0.000064 |         1 |     100 |  ascending order |
|  fluxsort |   100000 |   32 | 0.000525 | 0.000529 |         1 |     100 |  ascending order |
|  wolfsort |   100000 |   32 | 0.000067 | 0.000070 |         1 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001371 | 0.001436 |         1 |     100 |    ascending saw |
|  gridsort |   100000 |   32 | 0.001445 | 0.001459 |         1 |     100 |    ascending saw |
|   timsort |   100000 |   32 | 0.000842 | 0.000847 |         1 |     100 |    ascending saw |
|  quadsort |   100000 |   32 | 0.000879 | 0.000889 |         1 |     100 |    ascending saw |
|  fluxsort |   100000 |   32 | 0.002586 | 0.002604 |         1 |     100 |    ascending saw |
|  wolfsort |   100000 |   32 | 0.001199 | 0.001216 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003828 | 0.003857 |         1 |     100 |    generic order |
|  gridsort |   100000 |   32 | 0.001661 | 0.001693 |         1 |     100 |    generic order |
|   timsort |   100000 |   32 | 0.005591 | 0.005614 |         1 |     100 |    generic order |
|  quadsort |   100000 |   32 | 0.003777 | 0.003861 |         1 |     100 |    generic order |
|  fluxsort |   100000 |   32 | 0.001438 | 0.001450 |         1 |     100 |    generic order |
|  wolfsort |   100000 |   32 | 0.003788 | 0.003872 |         1 |     100 |    generic order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000900 | 0.000909 |         1 |     100 | descending order |
|  gridsort |   100000 |   32 | 0.000387 | 0.000399 |         1 |     100 | descending order |
|   timsort |   100000 |   32 | 0.000090 | 0.000093 |         1 |     100 | descending order |
|  quadsort |   100000 |   32 | 0.000067 | 0.000068 |         1 |     100 | descending order |
|  fluxsort |   100000 |   32 | 0.000526 | 0.000534 |         1 |     100 | descending order |
|  wolfsort |   100000 |   32 | 0.000072 | 0.000073 |         1 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001013 | 0.001029 |         1 |     100 |   descending saw |
|  gridsort |   100000 |   32 | 0.001020 | 0.001055 |         1 |     100 |   descending saw |
|   timsort |   100000 |   32 | 0.000460 | 0.000463 |         1 |     100 |   descending saw |
|  quadsort |   100000 |   32 | 0.000490 | 0.000499 |         1 |     100 |   descending saw |
|  fluxsort |   100000 |   32 | 0.000998 | 0.001025 |         1 |     100 |   descending saw |
|  wolfsort |   100000 |   32 | 0.000495 | 0.000503 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.002053 | 0.002104 |         1 |     100 |      random tail |
|  gridsort |   100000 |   32 | 0.001430 | 0.001440 |         1 |     100 |      random tail |
|   timsort |   100000 |   32 | 0.002004 | 0.002025 |         1 |     100 |      random tail |
|  quadsort |   100000 |   32 | 0.001567 | 0.001576 |         1 |     100 |      random tail |
|  fluxsort |   100000 |   32 | 0.001626 | 0.001639 |         1 |     100 |      random tail |
|  wolfsort |   100000 |   32 | 0.001000 | 0.001020 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003511 | 0.003558 |         1 |     100 |      random half |
|  gridsort |   100000 |   32 | 0.002555 | 0.002587 |         1 |     100 |      random half |
|   timsort |   100000 |   32 | 0.004037 | 0.004066 |         1 |     100 |      random half |
|  quadsort |   100000 |   32 | 0.003092 | 0.003116 |         1 |     100 |      random half |
|  fluxsort |   100000 |   32 | 0.002612 | 0.002624 |         1 |     100 |      random half |
|  wolfsort |   100000 |   32 | 0.001164 | 0.001181 |         1 |     100 |      random half |

</details>

The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. Each test was ran 100 times
and only the best run is reported. It measures the performance on random data with array sizes
ranging from 16 to 262144.

![Graph](/images/graph2.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |       16 |   32 | 0.001570 | 0.001653 |     16384 |     100 |        random 16 |
|  gridsort |       16 |   32 | 0.000675 | 0.000678 |     16384 |     100 |        random 16 |
|   timsort |       16 |   32 | 0.001410 | 0.001637 |     16384 |     100 |        random 16 |
|  quadsort |       16 |   32 | 0.000672 | 0.000673 |     16384 |     100 |        random 16 |
|  fluxsort |       16 |   32 | 0.000675 | 0.000680 |     16384 |     100 |        random 16 |
|  wolfsort |       16 |   32 | 0.000705 | 0.000713 |     16384 |     100 |        random 16 |
|           |          |      |          |          |           |         |                  |
|stablesort |       64 |   32 | 0.001641 | 0.001972 |      4096 |     100 |        random 64 |
|  gridsort |       64 |   32 | 0.001122 | 0.001168 |      4096 |     100 |        random 64 |
|   timsort |       64 |   32 | 0.002871 | 0.003387 |      4096 |     100 |        random 64 |
|  quadsort |       64 |   32 | 0.001126 | 0.001134 |      4096 |     100 |        random 64 |
|  fluxsort |       64 |   32 | 0.001130 | 0.001163 |      4096 |     100 |        random 64 |
|  wolfsort |       64 |   32 | 0.001146 | 0.001191 |      4096 |     100 |        random 64 |
|           |          |      |          |          |           |         |                  |
|stablesort |      256 |   32 | 0.001683 | 0.003924 |      1024 |     100 |       random 256 |
|  gridsort |      256 |   32 | 0.001589 | 0.002222 |      1024 |     100 |       random 256 |
|   timsort |      256 |   32 | 0.003314 | 0.003452 |      1024 |     100 |       random 256 |
|  quadsort |      256 |   32 | 0.001589 | 0.002152 |      1024 |     100 |       random 256 |
|  fluxsort |      256 |   32 | 0.001589 | 0.002154 |      1024 |     100 |       random 256 |
|  wolfsort |      256 |   32 | 0.001589 | 0.002324 |      1024 |     100 |       random 256 |
|           |          |      |          |          |           |         |                  |
|stablesort |     1024 |   32 | 0.006986 | 0.008080 |       256 |     100 |      random 1024 |
|  gridsort |     1024 |   32 | 0.004090 | 0.005028 |       256 |     100 |      random 1024 |
|   timsort |     1024 |   32 | 0.008288 | 0.010465 |       256 |     100 |      random 1024 |
|  quadsort |     1024 |   32 | 0.004099 | 0.004748 |       256 |     100 |      random 1024 |
|  fluxsort |     1024 |   32 | 0.003761 | 0.004560 |       256 |     100 |      random 1024 |
|  wolfsort |     1024 |   32 | 0.001953 | 0.002770 |       256 |     100 |      random 1024 |
|           |          |      |          |          |           |         |                  |
|stablesort |     4096 |   32 | 0.010790 | 0.010972 |        64 |     100 |      random 4096 |
|  gridsort |     4096 |   32 | 0.009922 | 0.009999 |        64 |     100 |      random 4096 |
|   timsort |     4096 |   32 | 0.015356 | 0.015427 |        64 |     100 |      random 4096 |
|  quadsort |     4096 |   32 | 0.010332 | 0.010461 |        64 |     100 |      random 4096 |
|  fluxsort |     4096 |   32 | 0.007293 | 0.007430 |        64 |     100 |      random 4096 |
|  wolfsort |     4096 |   32 | 0.002759 | 0.003281 |        64 |     100 |      random 4096 |
|           |          |      |          |          |           |         |                  |
|stablesort |    16384 |   32 | 0.013047 | 0.013111 |        16 |     100 |     random 16384 |
|  gridsort |    16384 |   32 | 0.011476 | 0.011522 |        16 |     100 |     random 16384 |
|   timsort |    16384 |   32 | 0.017545 | 0.017610 |        16 |     100 |     random 16384 |
|  quadsort |    16384 |   32 | 0.012937 | 0.012989 |        16 |     100 |     random 16384 |
|  fluxsort |    16384 |   32 | 0.009280 | 0.009318 |        16 |     100 |     random 16384 |
|  wolfsort |    16384 |   32 | 0.003394 | 0.003460 |        16 |     100 |     random 16384 |
|           |          |      |          |          |           |         |                  |
|stablesort |    65536 |   32 | 0.015032 | 0.015071 |         4 |     100 |     random 65536 |
|  gridsort |    65536 |   32 | 0.011924 | 0.012015 |         4 |     100 |     random 65536 |
|   timsort |    65536 |   32 | 0.019653 | 0.019737 |         4 |     100 |     random 65536 |
|  quadsort |    65536 |   32 | 0.014817 | 0.014869 |         4 |     100 |     random 65536 |
|  fluxsort |    65536 |   32 | 0.010533 | 0.010582 |         4 |     100 |     random 65536 |
|  wolfsort |    65536 |   32 | 0.003432 | 0.003476 |         4 |     100 |     random 65536 |
|           |          |      |          |          |           |         |                  |
|stablesort |   262144 |   32 | 0.017036 | 0.017102 |         1 |     100 |    random 262144 |
|  gridsort |   262144 |   32 | 0.013397 | 0.013457 |         1 |     100 |    random 262144 |
|   timsort |   262144 |   32 | 0.021748 | 0.021879 |         1 |     100 |    random 262144 |
|  quadsort |   262144 |   32 | 0.016776 | 0.016850 |         1 |     100 |    random 262144 |
|  fluxsort |   262144 |   32 | 0.010252 | 0.010303 |         1 |     100 |    random 262144 |
|  wolfsort |   262144 |   32 | 0.003569 | 0.003594 |         1 |     100 |    random 262144 |

</details>

Benchmark: quadsort vs qsort (mergesort)
----------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using gcc -O3 quadsort.c. Each test was ran 10 times
and only the best run is reported. It's generated by running the benchmark using
1000000 10 1 as the argument.

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|  quadsort |  1000000 |  128 | 0.181320 | 0.181632 |    19306961 |     random order |
|     qsort |  1000000 |  128 | 0.242565 | 0.247874 |    18674196 |     random order |

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|  quadsort |  1000000 |   64 | 0.101632 | 0.101842 |    19306344 |     random order |
|     qsort |  1000000 |   64 | 0.111756 | 0.113088 |    18674640 |     random order |

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|  quadsort |  1000000 |   32 | 0.092825 | 0.093127 |    19304717 |     random order |
|     qsort |  1000000 |   32 | 0.102933 | 0.104512 |    18674792 |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |   32 | 0.001957 | 0.002018 |      999999 |  ascending order |
|     qsort |  1000000 |   32 | 0.026737 | 0.027637 |     9884992 |  ascending order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |   32 | 0.013422 | 0.013537 |     4008111 |    ascending saw |
|     qsort |  1000000 |   32 | 0.034812 | 0.035417 |    10884978 |    ascending saw |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |   32 | 0.059303 | 0.059544 |    19241648 |    generic order |
|     qsort |  1000000 |   32 | 0.070272 | 0.071475 |    18618271 |    generic order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |   32 | 0.001706 | 0.001734 |      999999 | descending order |
|     qsort |  1000000 |   32 | 0.026258 | 0.026683 |    10066432 | descending order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |   32 | 0.017165 | 0.017406 |     9519110 |   descending saw |
|     qsort |  1000000 |   32 | 0.034933 | 0.035682 |    13906008 |   descending saw |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |   32 | 0.026106 | 0.026346 |     6785744 |      random tail |
|     qsort |  1000000 |   32 | 0.046105 | 0.047014 |    12248792 |      random tail |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |   32 | 0.050467 | 0.050667 |    11380780 |      random half |
|     qsort |  1000000 |   32 | 0.066697 | 0.067891 |    14529545 |      random half |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |   32 | 0.026015 | 0.026329 |    15078606 |           stable |
|     qsort |  1000000 |   32 | 0.035212 | 0.036771 |    14656048 |           stable |

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
it has fewer comparisons for ascending, generic, and descending order data. However, it performs
worse than quadsort on all tests except for generic order. Quicksort has an abysmal sorting
speed for wave order data.

Benchmark: quadsort vs qsort (mergesort) small arrays
-----------------------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using gcc -O3 quadsort.c. It's generated by running the
benchmark using 4096 0 0 as the argument. The benchmark is weighted with the number
of repetitions halving each time the number of items doubles.

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|  quadsort |        4 |   32 | 0.000329 | 0.000333 |           5 |       random 1-4 |
|     qsort |        4 |   32 | 0.000856 | 0.000875 |           5 |       random 1-4 |
|           |          |      |          |          |             |                  |
|  quadsort |        8 |   32 | 0.000541 | 0.000549 |          17 |       random 5-8 |
|     qsort |        8 |   32 | 0.001153 | 0.001177 |          17 |       random 5-8 |
|           |          |      |          |          |             |                  |
|  quadsort |       16 |   32 | 0.000656 | 0.000759 |          50 |      random 9-16 |
|     qsort |       16 |   32 | 0.001200 | 0.001214 |          46 |      random 9-16 |
|           |          |      |          |          |             |                  |
|  quadsort |       32 |   32 | 0.001011 | 0.001076 |         129 |     random 17-32 |
|     qsort |       32 |   32 | 0.001415 | 0.001805 |         121 |     random 17-32 |
|           |          |      |          |          |             |                  |
|  quadsort |       64 |   32 | 0.001145 | 0.001319 |         330 |     random 33-64 |
|     qsort |       64 |   32 | 0.002192 | 0.002639 |         309 |     random 33-64 |
|           |          |      |          |          |             |                  |
|  quadsort |      128 |   32 | 0.001519 | 0.001928 |         809 |    random 65-128 |
|     qsort |      128 |   32 | 0.003004 | 0.003556 |         745 |    random 65-128 |
|           |          |      |          |          |             |                  |
|  quadsort |      256 |   32 | 0.001609 | 0.002036 |        1879 |   random 129-256 |
|     qsort |      256 |   32 | 0.003557 | 0.003973 |        1738 |   random 129-256 |
|           |          |      |          |          |             |                  |
|  quadsort |      512 |   32 | 0.001920 | 0.002365 |        4273 |   random 257-512 |
|     qsort |      512 |   32 | 0.004207 | 0.004808 |        3968 |   random 257-512 |
|           |          |      |          |          |             |                  |
|  quadsort |     1024 |   32 | 0.001563 | 0.002485 |        5728 |  random 513-1024 |
|     qsort |     1024 |   32 | 0.003405 | 0.003722 |        5163 |  random 513-1024 |
|           |          |      |          |          |             |                  |
|  quadsort |     2048 |   32 | 0.002220 | 0.002662 |       10995 | random 1025-2048 |
|     qsort |     2048 |   32 | 0.003488 | 0.003711 |        9600 | random 1025-2048 |
|           |          |      |          |          |             |                  |
|  quadsort |     4096 |   32 | 0.002886 | 0.003203 |       23353 | random 2049-4096 |
|     qsort |     4096 |   32 | 0.003760 | 0.003934 |       20315 | random 2049-4096 |

As long as the L1 cache is not exhausted it is unlikely for quadsort to be beaten. When
sorting arrays under 1000 elements quadsort is between 1.8 and 2.3 times faster than qsort.
