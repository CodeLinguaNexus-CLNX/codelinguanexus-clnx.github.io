/*
This function is responsible for comparing two 80-bit extended precision floating-point numbers (floatx80) for equality while considering special cases such as infinity and NaN (Not a Number).

Here's the detail:
- The function first checks if either of the input floating-point numbers is a NaN or an infinity (exponent 0x7FFF with a non-zero fraction). If so, it sets the floating-point exception flag for invalid operation and returns 0, indicating inequality due to special cases.
- If neither input is a NaN or an infinity, the function compares the low and high parts of the floatx80 numbers to determine equality.
- It checks if the low parts are equal and if the high parts are equal, or if the low part of both numbers is zero and the modified high parts (shifted left by 1 bit and treated as 16-bit values) are also equal.

Needs to notice:
- The function performs bitwise operations and comparisons on the internal representation of the floatx80 numbers. Understanding of the IEEE 754 floating-point format and low-level bit manipulation is required to comprehend this function fully.
- Special consideration is given to NaN and infinity values, and proper handling of these special cases is important for accurate comparison results.
- The function raises a floating-point exception flag for invalid operation if either of the input numbers is a NaN or an infinity. The behavior of the float_status/status parameter should be clearly documented and understood when using this function.
*/
int floatx80_eq(floatx80 a, floatx80 b, float_status *status)

{



    if (    (    ( extractFloatx80Exp( a ) == 0x7FFF )

              && (uint64_t) ( extractFloatx80Frac( a )<<1 ) )

         || (    ( extractFloatx80Exp( b ) == 0x7FFF )

              && (uint64_t) ( extractFloatx80Frac( b )<<1 ) )

       ) {

        float_raise(float_flag_invalid, status);

        return 0;

    }

    return

           ( a.low == b.low )

        && (    ( a.high == b.high )

             || (    ( a.low == 0 )

                  && ( (uint16_t) ( ( a.high | b.high )<<1 ) == 0 ) )

           );



}
