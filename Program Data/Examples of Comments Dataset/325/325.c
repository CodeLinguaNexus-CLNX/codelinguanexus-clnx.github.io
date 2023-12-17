/*
This function is responsible for converting a tightly packed palette buffer to an RGB color value.

Here's the detail:
1. The function takes the bits per pixel (bpp) as well as a pointer to the buffer containing the palette data as input.
2. If the bits per pixel is 32, the function processes the buffer data to form an RGB color value. It performs bitwise operations and bit manipulations on the buffer content to extract the RGBA components and assigns them to the corresponding positions in the 32-bit RGB value.
3. If the bits per pixel is 16, the function processes the buffer data in a similar manner, generating an RGB color value by extracting the RGB components and composing them into a 32-bit RGB value.
4. The final RGB value is returned from the function.

Need's to notice:
1. The function performs bitwise operations and bit manipulations directly on the buffer data, assuming a specific endianness and data layout. It is essential to ensure that the input buffer is well-formed and contains the expected data to avoid potential undefined behavior or memory access violations.
2. The function assumes a certain structure and interpretation of the palette data based on the provided bits per pixel value. It is crucial to validate that the buffer data conforms to the expected format and bit layout to produce accurate RGB color values.
3. Considering the potential variations in endianness and platform-specific considerations, additional validation and adjustments may be necessary to ensure the correct interpretation of the palette data on different systems.
*/
static uint32_t tight_palette_buf2rgb(int bpp, const uint8_t *buf)

{

    uint32_t rgb = 0;



    if (bpp == 32) {

        rgb |= ((buf[0] & ~1) | !((buf[4] >> 3) & 1)) << 24;

        rgb |= ((buf[1] & ~1) | !((buf[4] >> 2) & 1)) << 16;

        rgb |= ((buf[2] & ~1) | !((buf[4] >> 1) & 1)) <<  8;

        rgb |= ((buf[3] & ~1) | !((buf[4] >> 0) & 1)) <<  0;

    }

    if (bpp == 16) {

        rgb |= ((buf[0] & ~1) | !((buf[2] >> 1) & 1)) << 8;

        rgb |= ((buf[1] & ~1) | !((buf[2] >> 0) & 1)) << 0;

    }

    return rgb;

}
