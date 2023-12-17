/*
This function is responsible for converting an 8-bit paletted image to a 24-bit BGR image using a provided palette.

Here's the detail: The function iterates through each pixel of the source image, and for each pixel, it uses the palette information to look up the corresponding BGR components and writes them to the destination buffer. It performs the conversion by accessing the palette using the index from the source image, and then writing the corresponding B, G, and R components to the destination buffer for each pixel.

Need's to notice: It's important to ensure that the source and destination buffers are properly allocated and have sufficient space to prevent buffer overflows. Additionally, the performance of this function might be a concern, and optimization techniques such as vectorization could be considered to improve the conversion speed, especially for a large number of pixels. Also, it would be advisable to include comments or documentation explaining the structure and format of the palette, as well as the expected layout of the source and destination buffers for future maintenance.
*/
void palette8tobgr24(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette)

{

	long i;

/*

	writes 1 byte o much and might cause alignment issues on some architectures?

	for(i=0; i<num_pixels; i++)

		((unsigned *)(&dst[i*3])) = ((unsigned *)palette)[ src[i] ];

*/

	for(i=0; i<num_pixels; i++)

	{

		//FIXME slow?

		dst[0]= palette[ src[i]*4+0 ];

		dst[1]= palette[ src[i]*4+1 ];

		dst[2]= palette[ src[i]*4+2 ];

		dst+= 3;

	}

}
