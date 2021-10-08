Intro
-----

This document describes a stable bottom-up adaptive merge sort named quadsort. A [visualisation](https://github.com/scandum/quadsort#visualization) and [benchmarks](https://github.com/scandum/quadsort#benchmark-quadsort-vs-stdstable_sort-vs-timsort) are available at the bottom.


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
    if (a > b)
    {
        swap = a; a = b; b = swap;
    }
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

The next optimization is to write the quad swap in such a way that we can
perform a simple check to see if the entire array was in reverse order,
if so, the sort is finished.

One final optimization, reverse order handling is only beneficial on
runs longer than 4 elements. When no reverse order run is detected
the next 4 elements are merged with the first 4 elements.

At the end of the loop the array has been turned into a series of ordered
blocks of 8 elements.

Parity merge
------------
The parity merge is a boundless merge used to turn blocks of 8 elements into
blocks of 32 elements. While it lacks adaptive properties it can be fully
unrolled. Performance wise it's slightly faster than insertion sort.

It takes advantage of the fact that if you have two n length arrays, you can
fully merge the two arrays by performing n merge operations on the start of
each array, and n merge operations on the end of each array. The arrays must
be of exactly equal length.

To sort 4 blocks of 8 elements into a sorted block of 32 elements takes 64
comparisons, 64 moves, and requires 32 elements of auxiliary memory.

Branchless parity merge
-----------------------
Since the parity merge can be unrolled it's very suitable for branchless
optimizations to speed up the sorting of random data. Another advantage
is that two separate memory regions can be accessed in the same loop with
no additional overhead. This makes the routine up to 2.5 times faster on
random data.

Quad merge
----------
In the first stage of quadsort the quad swap and parity merge are used to
pre-sort the array into sorted 32-element blocks as described above.

The second stage uses an approach similar to the parity merge, but it's
sorting blocks of 32, 128, 512, or more elements.

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
step 3, the blocks are merged back to main memory and we're left with 1 fully
sorted block. The following is a visualization of an array with 64random
elements getting sorted.

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

Branchless parity quad merge
-----------------
Due to the additional overhead of a branchless parity merge it's only faster
on random data. One additional comparison is performed during the quad merge
routine to determine whether it'd be faster to use a parity merge instead.

tail merge
----------
When sorting an array of 33 elements you end up with a sorted array of 32
elements and a sorted array of 1 element in the end. If a program sorts in
intervals it should pick an optimal array size (32, 128, 512, etc) to do so.

To work around this problem the remainder of the array is sorted using a tail
merge.

The main advantage of the tail merge is that it allows reducing the swap
space of quadsort to **n / 2** and that it has been optimized to merge arrays
of different lengths. It also simplifies the quad merge routine which only
needs to work on arrays of equal length.

rotate merge
------------
By using rotations the swap space of quadsort is reduced further from **n / 2**
to **n / 4**. Rotations can be performed with minimal performance loss by using
[monobound binary searches](https://github.com/scandum/binary_search) and [trinity rotations](https://github.com/scandum/rotate).

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

Memory
------
By default quadsort uses n / 4 swap memory. If memory allocation fails quadsort will switch to sorting in-place through rotations.

Performance
-----------
Quadsort is faster than quicksort for most data distributions.

Variants
--------
- [blitsort](https://github.com/scandum/blitsort) is a quadsort based rotate merge sort.

- [fluxsort](https://github.com/scandum/fluxsort) is a hybrid stable quicksort / quadsort with improved performance on random data. It is currently the fastest comparison sort for random data.

- [gridsort](https://github.com/scandum/gridsort) is a hybrid cubesort / quadsort. It is the fastest online sort and might be of interest to those interested in data structures.

- [twinsort](https://github.com/scandum/twinsort) is a simplified quadsort with a
much smaller code size. Twinsort might be of use to people who want to port or understand quadsort; it does not use
pointers or gotos.

- [wolfsort](https://github.com/scandum/wolfsort) is a hybrid radixsort / fluxsort with improved performance on random data. It's mostly a proof of concept that only work on unsigned 32 and 64 bit integers. It's possibly the fastest radix sort for 32 bit integers, overall fluxsort is faster for 64 bit integers.

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

<p align="center">
      <img src="https://raw.githubusercontent.com/scandum/quadsort/master/images/benchmark.gif">
</p>

There's also a [YouTube video of a java port of quadsort](https://www.youtube.com/watch?v=drSeVadf05M) in [ArrayV](https://github.com/Gaming32/ArrayV-v4.0) on a wide variety of data distrubtions.

Benchmark: quadsort vs std::stable_sort vs timsort
--------------------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. Stablesort is g++'s std:stablesort function. Each test was ran 100 times
on 100,000 elements. A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph1.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |      Reps | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |   100000 |   32 | 0.006081 | 0.006114 |         1 |     100 |     random order |
|  quadsort |   100000 |   32 | 0.002855 | 0.002869 |         1 |     100 |     random order |
|   timsort |   100000 |   32 | 0.007582 | 0.007609 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003050 | 0.003086 |         1 |     100 |     random % 100 |
|  quadsort |   100000 |   32 | 0.002353 | 0.002362 |         1 |     100 |     random % 100 |
|   timsort |   100000 |   32 | 0.004603 | 0.004634 |         1 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000786 | 0.000791 |         1 |     100 |        ascending |
|  quadsort |   100000 |   32 | 0.000065 | 0.000066 |         1 |     100 |        ascending |
|   timsort |   100000 |   32 | 0.000045 | 0.000045 |         1 |     100 |        ascending |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001468 | 0.001534 |         1 |     100 |    ascending saw |
|  quadsort |   100000 |   32 | 0.000893 | 0.000899 |         1 |     100 |    ascending saw |
|   timsort |   100000 |   32 | 0.000842 | 0.000849 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000868 | 0.000906 |         1 |     100 |       pipe organ |
|  quadsort |   100000 |   32 | 0.000229 | 0.000231 |         1 |     100 |       pipe organ |
|   timsort |   100000 |   32 | 0.000169 | 0.000171 |         1 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000899 | 0.000911 |         1 |     100 |       descending |
|  quadsort |   100000 |   32 | 0.000053 | 0.000054 |         1 |     100 |       descending |
|   timsort |   100000 |   32 | 0.000088 | 0.000092 |         1 |     100 |       descending |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001001 | 0.001027 |         1 |     100 |   descending saw |
|  quadsort |   100000 |   32 | 0.000414 | 0.000417 |         1 |     100 |   descending saw |
|   timsort |   100000 |   32 | 0.000301 | 0.000304 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.002147 | 0.002211 |         1 |     100 |      random tail |
|  quadsort |   100000 |   32 | 0.000898 | 0.000906 |         1 |     100 |      random tail |
|   timsort |   100000 |   32 | 0.001996 | 0.002015 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003606 | 0.003638 |         1 |     100 |      random half |
|  quadsort |   100000 |   32 | 0.001656 | 0.001662 |         1 |     100 |      random half |
|   timsort |   100000 |   32 | 0.004015 | 0.004031 |         1 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001105 | 0.001127 |         1 |     100 |  ascending tiles |
|  quadsort |   100000 |   32 | 0.000920 | 0.000929 |         1 |     100 |  ascending tiles |
|   timsort |   100000 |   32 | 0.000943 | 0.000979 |         1 |     100 |  ascending tiles |

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
|stablesort |        8 |   32 | 0.006216 | 0.006264 |     65536 |     100 |         random 8 |
|  quadsort |        8 |   32 | 0.002582 | 0.002616 |     65536 |     100 |         random 8 |
|   timsort |        8 |   32 | 0.006350 | 0.006527 |     65536 |     100 |         random 8 |
|           |          |      |          |          |           |         |                  |
|stablesort |       32 |   32 | 0.009274 | 0.009420 |     16384 |     100 |        random 32 |
|  quadsort |       32 |   32 | 0.004139 | 0.004197 |     16384 |     100 |        random 32 |
|   timsort |       32 |   32 | 0.013048 | 0.013224 |     16384 |     100 |        random 32 |
|           |          |      |          |          |           |         |                  |
|stablesort |      128 |   32 | 0.012983 | 0.013039 |      4096 |     100 |       random 128 |
|  quadsort |      128 |   32 | 0.005409 | 0.005488 |      4096 |     100 |       random 128 |
|   timsort |      128 |   32 | 0.019890 | 0.020001 |      4096 |     100 |       random 128 |
|           |          |      |          |          |           |         |                  |
|stablesort |      512 |   32 | 0.016939 | 0.017043 |      1024 |     100 |       random 512 |
|  quadsort |      512 |   32 | 0.006991 | 0.007075 |      1024 |     100 |       random 512 |
|   timsort |      512 |   32 | 0.024692 | 0.024837 |      1024 |     100 |       random 512 |
|           |          |      |          |          |           |         |                  |
|stablesort |     2048 |   32 | 0.020756 | 0.020830 |       256 |     100 |      random 2048 |
|  quadsort |     2048 |   32 | 0.008530 | 0.008581 |       256 |     100 |      random 2048 |
|   timsort |     2048 |   32 | 0.029035 | 0.029147 |       256 |     100 |      random 2048 |
|           |          |      |          |          |           |         |                  |
|stablesort |     8192 |   32 | 0.024663 | 0.024730 |        64 |     100 |      random 8192 |
|  quadsort |     8192 |   32 | 0.010177 | 0.010233 |        64 |     100 |      random 8192 |
|   timsort |     8192 |   32 | 0.033169 | 0.033263 |        64 |     100 |      random 8192 |
|           |          |      |          |          |           |         |                  |
|stablesort |    32768 |   32 | 0.028623 | 0.028716 |        16 |     100 |     random 32768 |
|  quadsort |    32768 |   32 | 0.011846 | 0.011891 |        16 |     100 |     random 32768 |
|   timsort |    32768 |   32 | 0.037333 | 0.037443 |        16 |     100 |     random 32768 |
|           |          |      |          |          |           |         |                  |
|stablesort |   131072 |   32 | 0.032613 | 0.032729 |         4 |     100 |    random 131072 |
|  quadsort |   131072 |   32 | 0.013536 | 0.013589 |         4 |     100 |    random 131072 |
|   timsort |   131072 |   32 | 0.041491 | 0.041605 |         4 |     100 |    random 131072 |
|           |          |      |          |          |           |         |                  |
|stablesort |   524288 |   32 | 0.036639 | 0.036762 |         1 |     100 |    random 524288 |
|  quadsort |   524288 |   32 | 0.015221 | 0.015301 |         1 |     100 |    random 524288 |
|   timsort |   524288 |   32 | 0.045715 | 0.045853 |         1 |     100 |    random 524288 |

</details>

Benchmark: quadsort vs qsort (mergesort)
----------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using gcc -O3 bench.c. Each test was ran 10 times. It's generated by running the benchmark using
1000000 10 1 as the argument. A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph3.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   64 | 0.237162 | 0.238677 |  18673752 |      10 |    random string |
|  quadsort |  1000000 |   64 | 0.192407 | 0.194719 |  19538735 |      10 |    random string |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |  128 | 0.238091 | 0.242881 |  18674976 |      10 |     random order |
|  quadsort |  1000000 |  128 | 0.153232 | 0.153598 |  19537512 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   64 | 0.112573 | 0.113432 |  18674640 |      10 |     random order |
|  quadsort |  1000000 |   64 | 0.055611 | 0.055827 |  19537674 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   32 | 0.102495 | 0.103431 |  18674792 |      10 |     random order |
|  quadsort |  1000000 |   32 | 0.046065 | 0.046385 |  19536519 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.023699 | 0.024186 |   9884992 |      10 |  ascending order |
|  quadsort |  1000000 |   32 | 0.001930 | 0.001952 |    999999 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.031943 | 0.032369 |  10884978 |      10 |    ascending saw |
|  quadsort |  1000000 |   32 | 0.011989 | 0.012192 |   4067986 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.070848 | 0.071297 |  18618271 |      10 |    generic order |
|  quadsort |  1000000 |   32 | 0.040046 | 0.040308 |  19526121 |      10 |    generic order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.028315 | 0.028653 |  10066432 |      10 | descending order |
|  quadsort |  1000000 |   32 | 0.001534 | 0.001558 |    999999 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.035975 | 0.036261 |  11066454 |      10 |   descending saw |
|  quadsort |  1000000 |   32 | 0.017375 | 0.017585 |   7261994 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.044251 | 0.044861 |  12248792 |      10 |      random tail |
|  quadsort |  1000000 |   32 | 0.016244 | 0.016387 |   6918301 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.064809 | 0.065331 |  14529545 |      10 |      random half |
|  quadsort |  1000000 |   32 | 0.028955 | 0.029254 |  11248721 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.048681 | 0.050011 |  14656048 |      10 |  ascending tiles |
|  quadsort |  1000000 |   32 | 0.045335 | 0.045865 |  15755690 |      10 |  ascending tiles |

</details>

In the benchmark above quadsort is compared against glibc qsort() using the same general
purpose interface and without any known unfair advantage, like inlining.

Benchmark: quadsort vs qsort (quicksort)
----------------------------------------
The following benchmark was on CYGWIN_NT-10.0-WOW gcc version 10.2.0.
The source code was compiled using gcc -O3 bench.c. Each test was ran 10 times. It's generated by
running the benchmark using 1000000 10 1 as the argument. A table with the best and average
time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph4.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   32 | 0.376701 | 0.379865 |  20117144 |      10 |    random string |
|  quadsort |  1000000 |   32 | 0.278388 | 0.279293 |  19537492 |      10 |    random string |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   96 | 0.208255 | 0.208759 |  20979145 |      10 |     random order |
|  quadsort |  1000000 |   96 | 0.171497 | 0.172393 |  19538311 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   64 | 0.169154 | 0.169911 |  20844955 |      10 |     random order |
|  quadsort |  1000000 |   64 | 0.104191 | 0.104681 |  19537566 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   32 | 0.142980 | 0.143391 |  20726941 |      10 |     random order |
|  quadsort |  1000000 |   32 | 0.067285 | 0.067616 |  19537443 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.007294 | 0.007402 |   3000004 |      10 |  ascending order |
|  quadsort |  1000000 |   32 | 0.002108 | 0.002257 |    999999 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.077101 | 0.077473 |  21065399 |      10 |    ascending saw |
|  quadsort |  1000000 |   32 | 0.017658 | 0.017906 |   4067910 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.041977 | 0.042386 |   6408586 |      10 |    generic order |
|  quadsort |  1000000 |   32 | 0.058442 | 0.058687 |  19524606 |      10 |    generic order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.010184 | 0.010283 |   4000015 |      10 | descending order |
|  quadsort |  1000000 |   32 | 0.001940 | 0.001949 |    999999 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.077490 | 0.077907 |  21148577 |      10 |   descending saw |
|  quadsort |  1000000 |   32 | 0.026893 | 0.027091 |   7118313 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.105767 | 0.105996 |  20437325 |      10 |      random tail |
|  quadsort |  1000000 |   32 | 0.022581 | 0.022778 |   6919488 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.124569 | 0.125127 |  20777317 |      10 |      random half |
|  quadsort |  1000000 |   32 | 0.042642 | 0.042813 |  11248934 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.015646 | 0.015894 |   4147713 |      10 |         unstable |
|  quadsort |  1000000 |   32 | 0.059448 | 0.060122 |  15755690 |      10 |  ascending tiles |

</details>

In this benchmark it becomes clear why quicksort is often preferred above a traditional mergesort,
it has fewer comparisons for ascending, generic, and descending order data. However, it performs
worse than quadsort on all tests except for generic order and ascending tiles. If quadsort was to
ignore stability it would beat quicksort on all tests.

Benchmark: quadsort vs qsort (mergesort) small arrays
-----------------------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using gcc -O3 bench.c. Each test was ran 100 times. It's generated by running the benchmark using
1000000 0 0 as the argument. The benchmark is weighted, meaning the number of repetitions
halves each time the number of items doubles.  A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph5.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |        8 |   32 | 0.009998 | 0.010752 |        33 |     100 |         random 8 |
|  quadsort |        8 |   32 | 0.005799 | 0.005833 |        21 |     100 |         random 8 |
|           |          |      |          |          |           |         |                  |
|     qsort |       32 |   32 | 0.018625 | 0.018901 |       140 |     100 |        random 32 |
|  quadsort |       32 |   32 | 0.010706 | 0.010843 |       147 |     100 |        random 32 |
|           |          |      |          |          |           |         |                  |
|     qsort |      128 |   32 | 0.026783 | 0.027012 |       876 |     100 |       random 128 |
|  quadsort |      128 |   32 | 0.011866 | 0.011986 |       849 |     100 |       random 128 |
|           |          |      |          |          |           |         |                  |
|     qsort |      512 |   32 | 0.034524 | 0.034698 |      4551 |     100 |       random 512 |
|  quadsort |      512 |   32 | 0.014263 | 0.014408 |      4387 |     100 |       random 512 |
|           |          |      |          |          |           |         |                  |
|     qsort |     2048 |   32 | 0.041968 | 0.042228 |     22893 |     100 |      random 2048 |
|  quadsort |     2048 |   32 | 0.017020 | 0.017171 |     21592 |     100 |      random 2048 |
|           |          |      |          |          |           |         |                  |
|     qsort |     8192 |   32 | 0.049305 | 0.049640 |    112874 |     100 |      random 8192 |
|  quadsort |     8192 |   32 | 0.019978 | 0.020134 |    102805 |     100 |      random 8192 |
|           |          |      |          |          |           |         |                  |
|     qsort |    32768 |   32 | 0.056610 | 0.056944 |    507210 |     100 |     random 32768 |
|  quadsort |    32768 |   32 | 0.022976 | 0.023068 |    477042 |     100 |     random 32768 |
|           |          |      |          |          |           |         |                  |
|     qsort |   131072 |   32 | 0.064370 | 0.064674 |   2306086 |     100 |    random 131072 |
|  quadsort |   131072 |   32 | 0.027260 | 0.027374 |   2170253 |     100 |    random 131072 |
|           |          |      |          |          |           |         |                  |
|     qsort |   524288 |   32 | 0.071765 | 0.072072 |  10406113 |     100 |    random 524288 |
|  quadsort |   524288 |   32 | 0.030140 | 0.030375 |   9729288 |     100 |    random 524288 |

</details>
