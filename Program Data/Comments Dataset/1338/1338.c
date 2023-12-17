/*
The provided code snippet appears to be a function named `sd_1d97_int` that takes an integer pointer `p`, and two integers `i0` and `i1` as parameters. It seems to perform some kind of 1D filtering or signal processing operation on the input array `p` within the range indicated by `i0` and `i1`.

Here's the breakdown:

This function is responsible for performing a 1D 9/7 integer-based lifting wavelet transform.

Here's the detail:
- It first checks if the range `i1` to `i0` is less than or equal to 1, and if so, it applies a specific transformation based on the value of `i0`.
- Next, it calls a function named `extend97_int` which seems to extend the array `p` to accommodate the filter operations. This assumed extension is likely a common part of filtering or wavelet transform operations on arrays.
- Then, it proceeds to perform a series of filter operations on the array elements within the given range, iterating over the array and modifying the elements based on specific arithmetic operations.

Needs to notice:
- When using this function, ensure that the input array `p` is valid and has sufficient memory allocated to accommodate the filter operations within the given range.
- The constants like `I_LFTG_X`, `I_LFTG_K`, `I_LFTG_ALPHA`, `I_LFTG_BETA`, `I_LFTG_GAMMA`, and `I_LFTG_DELTA` are used in the arithmetic operations, and their values and meanings should be understood in the context of the overall algorithm.
- The function seems to perform low-level signal processing, and it should be used with caution and a thorough understanding of its purpose and impact.
*/
static void sd_1d97_int(int *p, int i0, int i1)

{

    int i;



    if (i1 <= i0 + 1) {

        if (i0 == 1)

            p[1] = (p[1] * I_LFTG_X + (1<<15)) >> 16;

        else

            p[0] = (p[0] * I_LFTG_K + (1<<15)) >> 16;

        return;

    }



    extend97_int(p, i0, i1);

    i0++; i1++;



    for (i = i0/2 - 2; i < i1/2 + 1; i++)

        p[2 * i + 1] -= (I_LFTG_ALPHA * (p[2 * i]     + p[2 * i + 2]) + (1 << 15)) >> 16;

    for (i = i0/2 - 1; i < i1/2 + 1; i++)

        p[2 * i]     -= (I_LFTG_BETA  * (p[2 * i - 1] + p[2 * i + 1]) + (1 << 15)) >> 16;

    for (i = i0/2 - 1; i < i1/2; i++)

        p[2 * i + 1] += (I_LFTG_GAMMA * (p[2 * i]     + p[2 * i + 2]) + (1 << 15)) >> 16;

    for (i = i0/2; i < i1/2; i++)

        p[2 * i]     += (I_LFTG_DELTA * (p[2 * i - 1] + p[2 * i + 1]) + (1 << 15)) >> 16;

}
