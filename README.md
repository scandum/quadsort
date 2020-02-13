Intro
-----

This document describes a stable non-recursive merge sort named quadsort.


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

       A --> A,B --> A,C
       B --> A,B --> A,B,C,D
       C --> C,D --> A,B,C,D
       D --> C,D --> B,D

This allows for the distribution visualised above. The quad swap elimates 2
wasteful swap operations and allows to make several deductions about the final
ordering.

       A --> A,B --> A? --> C? --> A,C
       B --> A,B --> B? --> D? --> A,C
       C --> C,D --> C? --> A? --> B,D
       D --> C,D --> D? --> B? --> B,D

The next step is checking if the four temporary variables are in order or in
reverse-order. In the visualization above in-order is checked first and
reverse-order is checked second. If both checks fail the final arrangement is
known and two checks remain to determine the final order.

This eliminates 1 wasteful comparisons for in order sequences while creating
an additional comparison for random sequences. However, in the real world we
are rarely comparing truly random data, so in any instance where data is
more likely to be orderly than disorderly this shift in probability will give
an advantage.

By changing the comparison order probability can be shifted to give random
data the advantage. Regardless, there should be a slight overall advantage
due to the elimination of wasteful swapping. In C this looks as following:

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
       if (pts[0] <= pts[2])
       {
           val[0] = tmp[0];
           val[1] = tmp[2];
       }
       else
       {
           val[0] = tmp[2];
           val[1] = tmp[0];
       }

       if (pts[1] <= pts[3])
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

The second stage still uses an approach similar to the quad swap to detect
in order and reverse-order arrangements, but as it's sorting blocks of 4 or
more the final step needs to be handled like the traditional merge sort.

This can be visualized as following:

AAAA BBBB CCCC DDDD EEEE FFFF GGGG HHHH IIII JJJJ KKKK LLLL MMMM NNNN OOOO PPPP

ABCDABCDABCDABCD    EFGHEFGHEFGHEFGH    IJKLIJKLIJKLIJKL    MNOPMNOPMNOPMNOP

ABCDEFGHIJKLMNOPABCDEFGHIJKLMNOPABCDEFGHIJKLMNOPABCDEFGHIJKLMNOP

In the first row quadswap has been used to create 16 blocks of 4 sorted
elements each. In the second row quadsort has been used to create 4 blocks of
16 sorted elements each. In the last row we're left with 1 block of 64 sorted
elements. The following is a visualization.

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

In the case of partial order or partial reverse-order the comparisons in the
merge itself are unnecessary and subsequently omitted. The data still needs
to be swapped but this is a less computational intensive procedure.

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

Since a suboptimal array size isn't disastrous for quadsort I haven't
bothered implementing a solution.

Cache optimization
------------------

As currently implemented quadsort is not cache efficient for large arrays.
This can be alleviated by pre-sorting large blocks. This (and other hardware
specific optimizations) is not within the scope of the current implementation.
