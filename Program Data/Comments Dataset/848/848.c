/*
This function is responsible for performing a signed 64-bit integer division on an input 128-bit integer represented by the combined plow and phigh values.

Here's the detail of the function:
- The function begins by determining the sign of the 128-bit integer and the divisor (b) by checking the most significant bits of phigh and b respectively.
- If the 128-bit integer is negative, it invokes the neg128 function to obtain the two's complement representation of the integer.
- It then checks the sign of the divisor and converts it to a positive value if it's negative.
- The div64 function is then called to perform the division operation and update the plow and phigh values with the quotient and remainder.
- After the division, the function adjusts the signs of plow and phigh based on the signs of the original 128-bit integer and the divisor.

Needs to notice:
- Understanding the two's complement representation and sign manipulation is crucial for correctly handling negative numbers and performing the appropriate operations based on the signs of the operands.
- Proper handling of overflow and underflow conditions during the division operation is important for ensuring the correctness and accuracy of the division result.
- Careful consideration should be given to the handling of the signs of the 128-bit integer and the divisor to ensure the correct adjustment of signs after the division operation.
- Attention to the edge cases, such as division by zero and the minimum and maximum signed 64-bit integer values, is necessary to handle these cases appropriately and prevent undefined behavior.
*/
static void idiv64(uint64_t *plow, uint64_t *phigh, int64_t b)

{

    int sa, sb;

    sa = ((int64_t)*phigh < 0);

    if (sa)

        neg128(plow, phigh);

    sb = (b < 0);

    if (sb)

        b = -b;

    div64(plow, phigh, b);

    if (sa ^ sb)

        *plow = - *plow;

    if (sa)

        *phigh = - *phigh;

}
