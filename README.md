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

One last optimization is to write the quad swap in such a way that we can
perform a simple check to see if the entire array was in reverse order,
if so, the sort is finished. If not, we know the array has been turned
into a series of ordered blocks of 4 elements.

Parity merge
------------
The parity merge is a boundless merge used to turn blocks of 4 elements into
blocks of 16 elements. While it lacks adaptive properties it can be fully
unrolled. Performance wise it's slightly faster than insertion sort.

It takes advantage of the fact that if you have two n length arrays, you can
fully merge the two arrays by performing n merge operations on the start of
each array, and n merge operations on the end of each array. The arrays must
be of exactly equal length.

To sort 4 blocks of 4 elements into a sorted block of 16 elements takes 32
comparisons, 32 moves, and requires 16 elements of auxiliary memory.

Quad merge
----------
In the first stage of quadsort the quad swap and parity merge are used to
pre-sort the array into sorted 16-element blocks as described above.

The second stage uses an approach similar to the parity merge, but it's
sorting blocks of 16, 64, 256, or more elements.

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
Quadsort is faster than quicksort, with the exception of branchless quicksorts, where quadsort will be slower on mostly random data and faster on mostly ordered data.

Variants
--------
- [blitsort](https://github.com/scandum/blitsort) is a quadsort based rotate merge sort. It is currently the fastest O(1) memory stable sort.

- [fluxsort](https://github.com/scandum/fluxsort) is a hybrid partition / quadsort with improved performance on random data. It is currently the fastest stable comparison sort for random data.

- [gridsort](https://github.com/scandum/gridsort) is a hybrid cubesort / quadsort with improved performance on random data. 

- [twinsort](https://github.com/scandum/twinsort) is a simplified quadsort with a
much smaller code size. Twinsort might be of use to people who want to port or understand quadsort; it does not use
pointers or gotos.

- [wolfsort](https://github.com/scandum/wolfsort) is a hybrid radixsort / quadsort with improved performance on random data. It's mostly a proof of concept that only work on unsigned 32 and 64 bit integers.

Visualization
-------------

In the visualization below four tests are performed. The first test is on a random
distribution, the second on an ascending distribution, the third on a descending
distribution, and the fourth on an ascending distribution with a random tail.

The upper half shows the swap memory and the bottom half shows the main memory.
Colors are used to differentiate between skip, swap, merge, and copy operations.

![quadsort benchmark](/images/benchmark.gif)

Benchmark: quadsort vs std::stable_sort vs timsort
--------------------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. Each test was ran 100 times
and only the best run is reported. Stablesort is g++'s std:stablesort function.

![Graph](/images/graph1.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |   100000 |   32 | 0.006063 | 0.006094 |         1 |     100 |     random order |
|  quadsort |   100000 |   32 | 0.005168 | 0.005202 |         1 |     100 |     random order |
|   timsort |   100000 |   32 | 0.007630 | 0.007679 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.002987 | 0.003020 |         1 |     100 |     random % 100 |
|  quadsort |   100000 |   32 | 0.002239 | 0.002324 |         1 |     100 |     random % 100 |
|   timsort |   100000 |   32 | 0.004601 | 0.004628 |         1 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.004940 | 0.004964 |         1 |     100 |    random % 1000 |
|  quadsort |   100000 |   32 | 0.004085 | 0.004120 |         1 |     100 |    random % 1000 |
|   timsort |   100000 |   32 | 0.006563 | 0.006596 |         1 |     100 |    random % 1000 |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003848 | 0.003960 |         1 |     100 |   square % 10000 |
|  quadsort |   100000 |   32 | 0.003145 | 0.003273 |         1 |     100 |   square % 10000 |
|   timsort |   100000 |   32 | 0.005205 | 0.005279 |         1 |     100 |   square % 10000 |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000658 | 0.000689 |         1 |     100 |        ascending |
|  quadsort |   100000 |   32 | 0.000057 | 0.000060 |         1 |     100 |        ascending |
|   timsort |   100000 |   32 | 0.000044 | 0.000044 |         1 |     100 |        ascending |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001351 | 0.001422 |         1 |     100 |    ascending saw |
|  quadsort |   100000 |   32 | 0.000823 | 0.000831 |         1 |     100 |    ascending saw |
|   timsort |   100000 |   32 | 0.000843 | 0.000849 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000804 | 0.000837 |         1 |     100 |       pipe organ |
|  quadsort |   100000 |   32 | 0.000173 | 0.000174 |         1 |     100 |       pipe organ |
|   timsort |   100000 |   32 | 0.000175 | 0.000175 |         1 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000897 | 0.000909 |         1 |     100 |       descending |
|  quadsort |   100000 |   32 | 0.000050 | 0.000050 |         1 |     100 |       descending |
|   timsort |   100000 |   32 | 0.000101 | 0.000102 |         1 |     100 |       descending |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001046 | 0.001069 |         1 |     100 |   descending saw |
|  quadsort |   100000 |   32 | 0.000343 | 0.000346 |         1 |     100 |   descending saw |
|   timsort |   100000 |   32 | 0.000470 | 0.000473 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.002061 | 0.002124 |         1 |     100 |      random tail |
|  quadsort |   100000 |   32 | 0.001398 | 0.001410 |         1 |     100 |      random tail |
|   timsort |   100000 |   32 | 0.002002 | 0.002048 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003539 | 0.003587 |         1 |     100 |      random half |
|  quadsort |   100000 |   32 | 0.002764 | 0.002779 |         1 |     100 |      random half |
|   timsort |   100000 |   32 | 0.004030 | 0.004059 |         1 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000972 | 0.001000 |         1 |     100 |  ascending tiles |
|  quadsort |   100000 |   32 | 0.000873 | 0.000924 |         1 |     100 |  ascending tiles |
|   timsort |   100000 |   32 | 0.000835 | 0.000955 |         1 |     100 |  ascending tiles |

</details>

The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. Each test was ran 100 times
and only the best run is reported. It measures the performance on random data with array sizes
ranging from 16 to 262144.

![Graph](/images/graph2.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |       16 |   32 | 0.005139 | 0.005184 |     32768 |     100 |        random 16 |
|  quadsort |       16 |   32 | 0.002644 | 0.002673 |     32768 |     100 |        random 16 |
|   timsort |       16 |   32 | 0.004774 | 0.004951 |     32768 |     100 |        random 16 |
|           |          |      |          |          |           |         |                  |
|stablesort |       64 |   32 | 0.006731 | 0.007089 |      8192 |     100 |        random 64 |
|  quadsort |       64 |   32 | 0.004040 | 0.004069 |      8192 |     100 |        random 64 |
|   timsort |       64 |   32 | 0.008642 | 0.008754 |      8192 |     100 |        random 64 |
|           |          |      |          |          |           |         |                  |
|stablesort |      256 |   32 | 0.008643 | 0.008758 |      2048 |     100 |       random 256 |
|  quadsort |      256 |   32 | 0.006263 | 0.006306 |      2048 |     100 |       random 256 |
|   timsort |      256 |   32 | 0.011307 | 0.011409 |      2048 |     100 |       random 256 |
|           |          |      |          |          |           |         |                  |
|stablesort |     1024 |   32 | 0.010748 | 0.011014 |       512 |     100 |      random 1024 |
|  quadsort |     1024 |   32 | 0.007767 | 0.007821 |       512 |     100 |      random 1024 |
|   timsort |     1024 |   32 | 0.013527 | 0.013628 |       512 |     100 |      random 1024 |
|           |          |      |          |          |           |         |                  |
|stablesort |     4096 |   32 | 0.012671 | 0.012947 |       128 |     100 |      random 4096 |
|  quadsort |     4096 |   32 | 0.009483 | 0.009527 |       128 |     100 |      random 4096 |
|   timsort |     4096 |   32 | 0.015660 | 0.015753 |       128 |     100 |      random 4096 |
|           |          |      |          |          |           |         |                  |
|stablesort |    16384 |   32 | 0.014823 | 0.015470 |        32 |     100 |     random 16384 |
|  quadsort |    16384 |   32 | 0.011357 | 0.011512 |        32 |     100 |     random 16384 |
|   timsort |    16384 |   32 | 0.017775 | 0.018266 |        32 |     100 |     random 16384 |
|           |          |      |          |          |           |         |                  |
|stablesort |    65536 |   32 | 0.017074 | 0.017690 |         8 |     100 |     random 65536 |
|  quadsort |    65536 |   32 | 0.013263 | 0.013466 |         8 |     100 |     random 65536 |
|   timsort |    65536 |   32 | 0.019873 | 0.020335 |         8 |     100 |     random 65536 |
|           |          |      |          |          |           |         |                  |
|stablesort |   262144 |   32 | 0.019233 | 0.019449 |         2 |     100 |    random 262144 |
|  quadsort |   262144 |   32 | 0.015135 | 0.015246 |         2 |     100 |    random 262144 |
|   timsort |   262144 |   32 | 0.022009 | 0.022701 |         2 |     100 |    random 262144 |

</details>

Benchmark: quadsort vs qsort (mergesort)
----------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using gcc -O3 bench.c. Each test was ran 10 times
and only the best run is reported. It's generated by running the benchmark using
1000000 10 1 as the argument.

![Graph](/images/graph3.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |  128 | 0.237263 | 0.242199 |  18674976 |      10 |     random order |
|  quadsort |  1000000 |  128 | 0.168458 | 0.168711 |  19260870 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   64 | 0.112857 | 0.114569 |  18674640 |      10 |     random order |
|  quadsort |  1000000 |   64 | 0.096308 | 0.096631 |  19261015 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   32 | 0.101947 | 0.103925 |  18674792 |      10 |     random order |
|  quadsort |  1000000 |   32 | 0.084251 | 0.084521 |  19261638 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.023746 | 0.024488 |   9884992 |      10 |  ascending order |
|  quadsort |  1000000 |   32 | 0.001928 | 0.001981 |    999999 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.031912 | 0.032395 |  10884978 |      10 |    ascending saw |
|  quadsort |  1000000 |   32 | 0.013704 | 0.013870 |   4279431 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.071231 | 0.072123 |  18618271 |      10 |    generic order |
|  quadsort |  1000000 |   32 | 0.052140 | 0.052348 |  19179843 |      10 |    generic order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.028399 | 0.028844 |  10066432 |      10 | descending order |
|  quadsort |  1000000 |   32 | 0.001467 | 0.001491 |    999999 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.036018 | 0.036356 |  11066454 |      10 |   descending saw |
|  quadsort |  1000000 |   32 | 0.018384 | 0.018522 |   7004340 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.044350 | 0.045759 |  12248792 |      10 |      random tail |
|  quadsort |  1000000 |   32 | 0.023817 | 0.024053 |   6548176 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.064876 | 0.065870 |  14529545 |      10 |      random half |
|  quadsort |  1000000 |   32 | 0.046070 | 0.046349 |  11209526 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.045845 | 0.047625 |  14656048 |      10 |  ascending tiles |
|  quadsort |  1000000 |   32 | 0.037940 | 0.038255 |  13530041 |      10 |  ascending tiles |

</details>

In the benchmark above quadsort is compared against glibc qsort() using the same general
purpose interface and without any known unfair advantage, like inlining.

Benchmark: quadsort vs qsort (quicksort)
----------------------------------------
The following benchmark was on CYGWIN_NT-10.0-WOW gcc version 10.2.0.
The source code was compiled using gcc -O3 bench.c. Each test was ran 10 times
and only the best run is reported. It's generated by running the benchmark using
1000000 10 1 as the argument.

![Graph](/images/graph4.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   96 | 0.206247 | 0.206787 |  20979145 |      10 |     random order |
|  quadsort |  1000000 |   96 | 0.212315 | 0.213040 |  19260894 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   64 | 0.168733 | 0.169184 |  20844955 |      10 |     random order |
|  quadsort |  1000000 |   64 | 0.146324 | 0.146555 |  19259739 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   32 | 0.143262 | 0.143527 |  20726941 |      10 |     random order |
|  quadsort |  1000000 |   32 | 0.112600 | 0.112914 |  19260111 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.007340 | 0.007410 |   3000004 |      10 |  ascending order |
|  quadsort |  1000000 |   32 | 0.002122 | 0.002186 |    999999 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.076989 | 0.077301 |  21065399 |      10 |    ascending saw |
|  quadsort |  1000000 |   32 | 0.018997 | 0.019366 |   4279103 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.042156 | 0.042392 |   6408586 |      10 |    generic order |
|  quadsort |  1000000 |   32 | 0.067399 | 0.067658 |  19178026 |      10 |    generic order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.010168 | 0.010293 |   4000015 |      10 | descending order |
|  quadsort |  1000000 |   32 | 0.001903 | 0.001920 |    999999 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.077591 | 0.077817 |  21148577 |      10 |   descending saw |
|  quadsort |  1000000 |   32 | 0.025837 | 0.026072 |   6891812 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.105626 | 0.106028 |  20437325 |      10 |      random tail |
|  quadsort |  1000000 |   32 | 0.031476 | 0.031692 |   6548784 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.124832 | 0.125137 |  20777317 |      10 |      random half |
|  quadsort |  1000000 |   32 | 0.061860 | 0.062210 |  11208492 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.015595 | 0.015695 |   4147713 |      10 |         unstable |
|  quadsort |  1000000 |   32 | 0.051994 | 0.052355 |  13530041 |      10 |  ascending tiles |

</details>

In this benchmark it becomes clear why quicksort is often preferred above a traditional mergesort,
it has fewer comparisons for ascending, generic, and descending order data. However, it performs
worse than quadsort on all tests except for generic order and ascending tiles. If quadsort was to
ignore stability it would beat quicksort on all tests.

Benchmark: quadsort vs qsort (mergesort) small arrays
-----------------------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using gcc -O3 bench.c. Each test was ran 10 times
and only the best run is reported. It's generated by running the benchmark using
1024 0 0 as the argument. The benchmark is weighted, meaning the number of repetitions
halves each time the number of items doubles.

![Graph](/images/graph5.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |        4 |   32 | 0.007690 | 0.007851 |         5 |     100 |         random 4 |
|  quadsort |        4 |   32 | 0.003777 | 0.003819 |         5 |     100 |         random 4 |
|           |          |      |          |          |           |         |                  |
|     qsort |        8 |   32 | 0.009290 | 0.010139 |        17 |     100 |         random 8 |
|  quadsort |        8 |   32 | 0.005261 | 0.005300 |        17 |     100 |         random 8 |
|           |          |      |          |          |           |         |                  |
|     qsort |       16 |   32 | 0.011707 | 0.012550 |        46 |     100 |        random 16 |
|  quadsort |       16 |   32 | 0.007616 | 0.007683 |        72 |     100 |        random 16 |
|           |          |      |          |          |           |         |                  |
|     qsort |       32 |   32 | 0.014411 | 0.015177 |       121 |     100 |        random 32 |
|  quadsort |       32 |   32 | 0.008152 | 0.008287 |       134 |     100 |        random 32 |
|           |          |      |          |          |           |         |                  |
|     qsort |       64 |   32 | 0.016926 | 0.018011 |       309 |     100 |        random 64 |
|  quadsort |       64 |   32 | 0.010751 | 0.010884 |       336 |     100 |        random 64 |
|           |          |      |          |          |           |         |                  |
|     qsort |      128 |   32 | 0.019462 | 0.020591 |       745 |     100 |       random 128 |
|  quadsort |      128 |   32 | 0.013592 | 0.013735 |       809 |     100 |       random 128 |
|           |          |      |          |          |           |         |                  |
|     qsort |      256 |   32 | 0.022029 | 0.023204 |      1738 |     100 |       random 256 |
|  quadsort |      256 |   32 | 0.016319 | 0.016458 |      1908 |     100 |       random 256 |
|           |          |      |          |          |           |         |                  |
|     qsort |      512 |   32 | 0.024775 | 0.026182 |      3968 |     100 |       random 512 |
|  quadsort |      512 |   32 | 0.018402 | 0.018537 |      4293 |     100 |       random 512 |
|           |          |      |          |          |           |         |                  |
|     qsort |     1024 |   32 | 0.027251 | 0.028544 |      8962 |     100 |      random 1024 |
|  quadsort |     1024 |   32 | 0.020347 | 0.020480 |      9568 |     100 |      random 1024 |
|           |          |      |          |          |           |         |                  |
|     qsort |     2048 |   32 | 0.029926 | 0.031449 |     19962 |     100 |      random 2048 |
|  quadsort |     2048 |   32 | 0.022652 | 0.023094 |     21099 |     100 |      random 2048 |
|           |          |      |          |          |           |         |                  |
|     qsort |     4096 |   32 | 0.032338 | 0.034431 |     43966 |     100 |      random 4096 |
|  quadsort |     4096 |   32 | 0.024869 | 0.025438 |     46228 |     100 |      random 4096 |
|           |          |      |          |          |           |         |                  |
|     qsort |     8192 |   32 | 0.034948 | 0.036151 |     96149 |     100 |      random 8192 |
|  quadsort |     8192 |   32 | 0.027317 | 0.027429 |    100627 |     100 |      random 8192 |
|           |          |      |          |          |           |         |                  |
|     qsort |    16384 |   32 | 0.037536 | 0.038818 |    208702 |     100 |     random 16384 |
|  quadsort |    16384 |   32 | 0.029744 | 0.029838 |    217658 |     100 |     random 16384 |
|           |          |      |          |          |           |         |                  |
|     qsort |    32768 |   32 | 0.040057 | 0.041405 |    450105 |     100 |     random 32768 |
|  quadsort |    32768 |   32 | 0.032199 | 0.032309 |    468066 |     100 |     random 32768 |
|           |          |      |          |          |           |         |                  |
|     qsort |    65536 |   32 | 0.042643 | 0.043925 |    965773 |     100 |     random 65536 |
|  quadsort |    65536 |   32 | 0.034603 | 0.034697 |   1001505 |     100 |     random 65536 |
|           |          |      |          |          |           |         |                  |
|     qsort |   131072 |   32 | 0.045241 | 0.046523 |   2062601 |     100 |    random 131072 |
|  quadsort |   131072 |   32 | 0.037075 | 0.037214 |   2134362 |     100 |    random 131072 |
|           |          |      |          |          |           |         |                  |
|     qsort |   262144 |   32 | 0.047816 | 0.048797 |   4387116 |     100 |    random 262144 |
|  quadsort |   262144 |   32 | 0.039469 | 0.039654 |   4530392 |     100 |    random 262144 |
|           |          |      |          |          |           |         |                  |
|     qsort |   524288 |   32 | 0.050317 | 0.051200 |   9298689 |     100 |    random 524288 |
|  quadsort |   524288 |   32 | 0.041799 | 0.041972 |   9584959 |     100 |    random 524288 |

</details>
