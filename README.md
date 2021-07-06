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
- [gridsort](https://github.com/scandum/gridsort) is a hybrid cubesort / quadsort with improved performance on random data. It is currently the fastest stable comparison sort for random data.

- [blitsort](https://github.com/scandum/blitsort) is a quadsort based rotate merge sort. It is currently the fastest O(1) memory sort.

- [twinsort](https://github.com/scandum/twinsort) is a simplified quadsort with a
much smaller code size. Twinsort might be of use to people who want to port or understand quadsort; it does not use
pointers or gotos.

- [wolfsort](https://github.com/scandum/wolfsort) is a hybrid radixsort / quadsort with improved performance on random data. It's mostly a proof of concept that only work on unsigned 32 and 64 bit integers.

- [fluxsort](https://github.com/scandum/wolfsort) is a hybrid partition / quadsort with improved performance on random data. It doesn't have its own project page and is just tossed in with the other sorts in the wolfsort benchmark. It has performance similar to gridsort.

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
|stablesort |   100000 |   32 | 0.006082 | 0.006121 |         1 |     100 |     random order |
|  quadsort |   100000 |   32 | 0.005745 | 0.005782 |         1 |     100 |     random order |
|   timsort |   100000 |   32 | 0.007605 | 0.007645 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000780 | 0.000824 |         1 |     100 |  ascending order |
|  quadsort |   100000 |   32 | 0.000052 | 0.000053 |         1 |     100 |  ascending order |
|   timsort |   100000 |   32 | 0.000044 | 0.000045 |         1 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001464 | 0.001556 |         1 |     100 |    ascending saw |
|  quadsort |   100000 |   32 | 0.000845 | 0.000854 |         1 |     100 |    ascending saw |
|   timsort |   100000 |   32 | 0.000849 | 0.000859 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003936 | 0.003983 |         1 |     100 |    generic order |
|  quadsort |   100000 |   32 | 0.003620 | 0.003642 |         1 |     100 |    generic order |
|   timsort |   100000 |   32 | 0.005556 | 0.005596 |         1 |     100 |    generic order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000900 | 0.000918 |         1 |     100 | descending order |
|  quadsort |   100000 |   32 | 0.000050 | 0.000052 |         1 |     100 | descending order |
|   timsort |   100000 |   32 | 0.000101 | 0.000104 |         1 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001091 | 0.001123 |         1 |     100 |   descending saw |
|  quadsort |   100000 |   32 | 0.000357 | 0.000365 |         1 |     100 |   descending saw |
|   timsort |   100000 |   32 | 0.000458 | 0.000464 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.002150 | 0.002217 |         1 |     100 |      random tail |
|  quadsort |   100000 |   32 | 0.001531 | 0.001543 |         1 |     100 |      random tail |
|   timsort |   100000 |   32 | 0.002000 | 0.002030 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003615 | 0.003674 |         1 |     100 |      random half |
|  quadsort |   100000 |   32 | 0.003034 | 0.003064 |         1 |     100 |      random half |
|   timsort |   100000 |   32 | 0.004027 | 0.004061 |         1 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001106 | 0.001130 |         1 |     100 |  ascending tiles |
|  quadsort |   100000 |   32 | 0.000800 | 0.000811 |         1 |     100 |  ascending tiles |
|   timsort |   100000 |   32 | 0.000824 | 0.000973 |         1 |     100 |  ascending tiles |

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
|stablesort |        8 |   32 | 0.003131 | 0.003163 |     32768 |     100 |         random 8 |
|  quadsort |        8 |   32 | 0.001940 | 0.001956 |     32768 |     100 |         random 8 |
|   timsort |        8 |   32 | 0.003165 | 0.003389 |     32768 |     100 |         random 8 |
|           |          |      |          |          |           |         |                  |
|stablesort |       16 |   32 | 0.003880 | 0.003923 |     16384 |     100 |        random 16 |
|  quadsort |       16 |   32 | 0.003229 | 0.003248 |     16384 |     100 |        random 16 |
|   timsort |       16 |   32 | 0.004531 | 0.004646 |     16384 |     100 |        random 16 |
|           |          |      |          |          |           |         |                  |
|stablesort |       64 |   32 | 0.005068 | 0.005469 |      4096 |     100 |        random 64 |
|  quadsort |       64 |   32 | 0.004159 | 0.004376 |      4096 |     100 |        random 64 |
|   timsort |       64 |   32 | 0.007023 | 0.007769 |      4096 |     100 |        random 64 |
|           |          |      |          |          |           |         |                  |
|stablesort |      256 |   32 | 0.007124 | 0.007369 |      1024 |     100 |       random 256 |
|  quadsort |      256 |   32 | 0.005686 | 0.006134 |      1024 |     100 |       random 256 |
|   timsort |      256 |   32 | 0.009631 | 0.010303 |      1024 |     100 |       random 256 |
|           |          |      |          |          |           |         |                  |
|stablesort |     1024 |   32 | 0.009412 | 0.009486 |       256 |     100 |      random 1024 |
|  quadsort |     1024 |   32 | 0.008142 | 0.008386 |       256 |     100 |      random 1024 |
|   timsort |     1024 |   32 | 0.012786 | 0.013002 |       256 |     100 |      random 1024 |
|           |          |      |          |          |           |         |                  |
|stablesort |     4096 |   32 | 0.011340 | 0.011397 |        64 |     100 |      random 4096 |
|  quadsort |     4096 |   32 | 0.010569 | 0.010634 |        64 |     100 |      random 4096 |
|   timsort |     4096 |   32 | 0.015296 | 0.015402 |        64 |     100 |      random 4096 |
|           |          |      |          |          |           |         |                  |
|stablesort |    16384 |   32 | 0.013342 | 0.013395 |        16 |     100 |     random 16384 |
|  quadsort |    16384 |   32 | 0.012644 | 0.012713 |        16 |     100 |     random 16384 |
|   timsort |    16384 |   32 | 0.017462 | 0.017559 |        16 |     100 |     random 16384 |
|           |          |      |          |          |           |         |                  |
|stablesort |    65536 |   32 | 0.015315 | 0.015391 |         4 |     100 |     random 65536 |
|  quadsort |    65536 |   32 | 0.014576 | 0.014645 |         4 |     100 |     random 65536 |
|   timsort |    65536 |   32 | 0.019557 | 0.019655 |         4 |     100 |     random 65536 |
|           |          |      |          |          |           |         |                  |
|stablesort |   262144 |   32 | 0.017350 | 0.017460 |         1 |     100 |    random 262144 |
|  quadsort |   262144 |   32 | 0.016530 | 0.016616 |         1 |     100 |    random 262144 |
|   timsort |   262144 |   32 | 0.021677 | 0.021792 |         1 |     100 |    random 262144 |

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
|     qsort |  1000000 |  128 | 0.235768 | 0.239245 |  18674196 |      10 |     random order |
|  quadsort |  1000000 |  128 | 0.174400 | 0.175445 |  18913990 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   64 | 0.111600 | 0.112593 |  18674640 |      10 |     random order |
|  quadsort |  1000000 |   64 | 0.099463 | 0.099995 |  18913145 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   32 | 0.104453 | 0.105502 |  18674792 |      10 |     random order |
|  quadsort |  1000000 |   32 | 0.091874 | 0.092567 |  18911618 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.027044 | 0.028325 |   9884992 |      10 |  ascending order |
|  quadsort |  1000000 |   32 | 0.001743 | 0.001858 |    999999 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.034983 | 0.036321 |  10884978 |      10 |    ascending saw |
|  quadsort |  1000000 |   32 | 0.013020 | 0.013133 |   4008060 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.072557 | 0.073006 |  18618271 |      10 |    generic order |
|  quadsort |  1000000 |   32 | 0.058651 | 0.059317 |  18854788 |      10 |    generic order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.026516 | 0.027000 |  10066432 |      10 | descending order |
|  quadsort |  1000000 |   32 | 0.001532 | 0.001559 |    999999 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.034386 | 0.034958 |  11066454 |      10 |   descending saw |
|  quadsort |  1000000 |   32 | 0.018517 | 0.018657 |   7402112 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.046482 | 0.047802 |  12248792 |      10 |      random tail |
|  quadsort |  1000000 |   32 | 0.026082 | 0.026321 |   6687684 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.067025 | 0.068773 |  14529545 |      10 |      random half |
|  quadsort |  1000000 |   32 | 0.050106 | 0.050525 |  11184427 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.049381 | 0.053382 |  14656048 |      10 |  ascending tiles |
|  quadsort |  1000000 |   32 | 0.044197 | 0.045308 |  14766116 |      10 |  ascending tiles |

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
|     qsort |  1000000 |   96 | 0.206188 | 0.206527 |  20935178 |      10 |     random order |
|  quadsort |  1000000 |   96 | 0.204535 | 0.205050 |  18912237 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   64 | 0.168539 | 0.169067 |  20774835 |      10 |     random order |
|  quadsort |  1000000 |   64 | 0.142734 | 0.143287 |  18914064 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |  1000000 |   32 | 0.143448 | 0.143770 |  20656249 |      10 |     random order |
|  quadsort |  1000000 |   32 | 0.121374 | 0.121873 |  18912751 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.006862 | 0.006983 |   3000004 |      10 |  ascending order |
|  quadsort |  1000000 |   32 | 0.002040 | 0.002132 |    999999 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.075683 | 0.075918 |  21154228 |      10 |    ascending saw |
|  quadsort |  1000000 |   32 | 0.019680 | 0.019837 |   4007748 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.040867 | 0.041125 |   6261279 |      10 |    generic order |
|  quadsort |  1000000 |   32 | 0.075495 | 0.075964 |  18853566 |      10 |    generic order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.009380 | 0.009484 |   4000015 |      10 | descending order |
|  quadsort |  1000000 |   32 | 0.001968 | 0.001998 |    999999 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.076841 | 0.077036 |  21944667 |      10 |   descending saw |
|  quadsort |  1000000 |   32 | 0.026407 | 0.026593 |   7189182 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.103838 | 0.104074 |  20627766 |      10 |      random tail |
|  quadsort |  1000000 |   32 | 0.034555 | 0.034782 |   6688107 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.123635 | 0.124209 |  20703791 |      10 |      random half |
|  quadsort |  1000000 |   32 | 0.066642 | 0.067130 |  11185919 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |  1000000 |   32 | 0.015749 | 0.016073 |   4147713 |      10 |  ascending tiles |
|  quadsort |  1000000 |   32 | 0.055769 | 0.056198 |  14766116 |      10 |  ascending tiles |

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
|     qsort |        4 |   32 | 0.012951 | 0.015632 |         5 |     100 |         random 4 |
|  quadsort |        4 |   32 | 0.005826 | 0.007321 |         5 |     100 |         random 4 |
|           |          |      |          |          |           |         |                  |
|     qsort |        8 |   32 | 0.018523 | 0.020900 |        17 |     100 |         random 8 |
|  quadsort |        8 |   32 | 0.010304 | 0.010358 |        17 |     100 |         random 8 |
|           |          |      |          |          |           |         |                  |
|     qsort |       16 |   32 | 0.022189 | 0.025041 |        46 |     100 |        random 16 |
|  quadsort |       16 |   32 | 0.017525 | 0.017635 |        51 |     100 |        random 16 |
|           |          |      |          |          |           |         |                  |
|     qsort |       32 |   32 | 0.025841 | 0.030131 |       121 |     100 |        random 32 |
|  quadsort |       32 |   32 | 0.020234 | 0.021181 |       129 |     100 |        random 32 |
|           |          |      |          |          |           |         |                  |
|     qsort |       64 |   32 | 0.031851 | 0.035673 |       309 |     100 |        random 64 |
|  quadsort |       64 |   32 | 0.024876 | 0.026862 |       328 |     100 |        random 64 |
|           |          |      |          |          |           |         |                  |
|     qsort |      128 |   32 | 0.035650 | 0.041490 |       745 |     100 |       random 128 |
|  quadsort |      128 |   32 | 0.026417 | 0.030621 |       782 |     100 |       random 128 |
|           |          |      |          |          |           |         |                  |
|     qsort |      256 |   32 | 0.042116 | 0.047296 |      1738 |     100 |       random 256 |
|  quadsort |      256 |   32 | 0.033521 | 0.036011 |      1818 |     100 |       random 256 |
|           |          |      |          |          |           |         |                  |
|     qsort |      512 |   32 | 0.049071 | 0.052659 |      3968 |     100 |       random 512 |
|  quadsort |      512 |   32 | 0.038291 | 0.040567 |      4156 |     100 |       random 512 |
|           |          |      |          |          |           |         |                  |
|     qsort |     1024 |   32 | 0.055021 | 0.058019 |      8962 |     100 |      random 1024 |
|  quadsort |     1024 |   32 | 0.044939 | 0.045974 |      9312 |     100 |      random 1024 |
|           |          |      |          |          |           |         |                  |
|     qsort |     2048 |   32 | 0.060945 | 0.063426 |     19962 |     100 |      random 2048 |
|  quadsort |     2048 |   32 | 0.050165 | 0.050943 |     20640 |     100 |      random 2048 |
|           |          |      |          |          |           |         |                  |
|     qsort |     4096 |   32 | 0.067210 | 0.068730 |     43966 |     100 |      random 4096 |
|  quadsort |     4096 |   32 | 0.055807 | 0.056008 |     45358 |     100 |      random 4096 |
|           |          |      |          |          |           |         |                  |
|     qsort |     8192 |   32 | 0.072263 | 0.074081 |     96149 |     100 |      random 8192 |
|  quadsort |     8192 |   32 | 0.060956 | 0.061111 |     98928 |     100 |      random 8192 |
|           |          |      |          |          |           |         |                  |
|     qsort |    16384 |   32 | 0.077036 | 0.079203 |    208702 |     100 |     random 16384 |
|  quadsort |    16384 |   32 | 0.066100 | 0.066276 |    214366 |     100 |     random 16384 |
|           |          |      |          |          |           |         |                  |
|     qsort |    32768 |   32 | 0.082605 | 0.084376 |    450105 |     100 |     random 32768 |
|  quadsort |    32768 |   32 | 0.071050 | 0.071203 |    461448 |     100 |     random 32768 |
|           |          |      |          |          |           |         |                  |
|     qsort |    65536 |   32 | 0.088038 | 0.089666 |    965773 |     100 |     random 65536 |
|  quadsort |    65536 |   32 | 0.076240 | 0.076393 |    988248 |     100 |     random 65536 |
|           |          |      |          |          |           |         |                  |
|     qsort |   131072 |   32 | 0.093045 | 0.094960 |   2062601 |     100 |    random 131072 |
|  quadsort |   131072 |   32 | 0.081188 | 0.081355 |   2107924 |     100 |    random 131072 |
|           |          |      |          |          |           |         |                  |
|     qsort |   262144 |   32 | 0.098222 | 0.099887 |   4387116 |     100 |    random 262144 |
|  quadsort |   262144 |   32 | 0.086321 | 0.086507 |   4477623 |     100 |    random 262144 |
|           |          |      |          |          |           |         |                  |
|     qsort |   524288 |   32 | 0.103319 | 0.105183 |   9298689 |     100 |    random 524288 |
|  quadsort |   524288 |   32 | 0.091543 | 0.091808 |   9479486 |     100 |    random 524288 |

</details>
