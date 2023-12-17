/*
This static function is responsible for performing DC coefficient addition after 4x4 inverse discrete cosine transform (IDCT) on the chroma (UV) blocks in a VP8 video stream.

Here's the detail:
- It takes a pointer to the destination buffer `dst`, a 4x4 block of 16-bit coefficients `block`, and the stride of the destination buffer as input parameters.
- The function then calls the `vp8_idct_dc_add_c` function four times, each time operating on a 4x4 block of the destination buffer.
- The offsets used in the `dst` pointer calculation indicate the positions within the destination buffer where the DC coefficients are added for each 4x4 block.

Needs to notice:
- The `vp8_idct_dc_add_c` function is assumed to correctly handle the addition of DC coefficients and be compatible with the structure of the `block` data.
- It's important to ensure that the destination buffer `dst` is properly allocated and has sufficient space to accommodate the resulting data after the DC coefficient additions.
- The use of pointer arithmetic to calculate the positions within the destination buffer should be carefully reviewed to avoid potential pointer arithmetic issues, especially related to buffer overflows or incorrect memory accesses.
*/
static void vp8_idct_dc_add4uv_c(uint8_t* dst, int16_t block[4][16], ptrdiff_t stride)

{

    vp8_idct_dc_add_c(dst+stride*0+0, block[0], stride);

    vp8_idct_dc_add_c(dst+stride*0+4, block[1], stride);

    vp8_idct_dc_add_c(dst+stride*4+0, block[2], stride);

    vp8_idct_dc_add_c(dst+stride*4+4, block[3], stride);

}
