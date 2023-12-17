/*
The provided code snippet appears to be a function that converts RGB32 pixel data to Y component (luminance) using the specified coefficients and bit shifts. It seems to be written in C or a similar language.

Here's the breakdown:

This function is responsible for converting a line of RGB32 pixels to the Y component using specific conversion coefficients.

Here's the detail:
- It iterates through each pixel in the source `src` buffer and extracts the individual RGB components (red, green, and blue) by bit manipulation.
- It then applies a conversion formula using the extracted RGB components and the specified conversion coefficients (RY, GY, BY, and RGB2YUV_SHIFT) to calculate the Y component.
- The resulting Y component is stored in the destination `dst` buffer at the corresponding position.

The function uses the following conversion formula:
Y = (RY * R + GY * G + BY * B + (33 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT

Needs to notice:
- When using this function, ensure that the source and destination buffers are properly allocated and have sufficient memory to accommodate the pixel data.
- The values of RY, GY, BY, and RGB2YUV_SHIFT and their impact on the conversion should be understood in the context of the overall image processing pipeline.
- The function uses bit manipulation and bit shift operations, so it should be used with caution and attention to endianness issues on different platforms.
- The function is likely a part of a larger image processing or video encoding/decoding pipeline and should be used in accordance with the requirements of the specific application.
*/
static inline void RENAME(rgb32ToY)(uint8_t *dst, uint8_t *src, int width)

{

	int i;

	for(i=0; i<width; i++)

	{

		int r=  ((uint32_t*)src)[i]&0xFF;

		int g= (((uint32_t*)src)[i]>>8)&0xFF;

		int b= (((uint32_t*)src)[i]>>16)&0xFF;



		dst[i]= ((RY*r + GY*g + BY*b + (33<<(RGB2YUV_SHIFT-1)) )>>RGB2YUV_SHIFT);

	}

}
