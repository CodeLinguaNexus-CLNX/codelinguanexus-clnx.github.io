/*
This function is responsible for copying packed 12-bit pixel data from an FFmpeg frame to an OpenJPEG image, converting the data format as necessary to match the requirements of the OpenJPEG library.

Here's the detail: The function begins by iterating through the image components to ensure that the frame's linesize is sufficient for the image data. It then proceeds to iterate over each component and each pixel row of the output image, copying and converting the pixel data from the FFmpeg frame to the OpenJPEG image. The conversion involves extracting 12-bit pixel values from the frame, aligning them appropriately, and populating the corresponding locations in the image data structure.

Needs to notice: It's important to verify that the input frame and image data structures are valid and initialized prior to calling this function. Additionally, the function assumes that the components of the OpenJPEG image are already initialized with the correct dimensions and other parameters. Proper error checking and handling should be added to account for potential discrepancies between the frame and image data, ensuring that unexpected behavior or memory access violations are avoided during the data copying process.
*/
static int libopenjpeg_copy_packed12(AVCodecContext *avctx, const AVFrame *frame, opj_image_t *image)

{

    int compno;

    int x, y;

    int *image_line;

    int frame_index;

    const int numcomps  = image->numcomps;

    uint16_t *frame_ptr = (uint16_t *)frame->data[0];



    for (compno = 0; compno < numcomps; ++compno) {

        if (image->comps[compno].w > frame->linesize[0] / numcomps) {

            av_log(avctx, AV_LOG_ERROR, "Error: frame's linesize is too small for the image\n");

            return 0;

        }

    }



    for (compno = 0; compno < numcomps; ++compno) {

        for (y = 0; y < avctx->height; ++y) {

            image_line = image->comps[compno].data + y * image->comps[compno].w;

            frame_index = y * (frame->linesize[0] / 2) + compno;

            for (x = 0; x < avctx->width; ++x) {

                image_line[x] = frame_ptr[frame_index] >> 4;

                frame_index += numcomps;

            }

            for (; x < image->comps[compno].w; ++x) {

                image_line[x] = image_line[x - 1];

            }

        }

        for (; y < image->comps[compno].h; ++y) {

            image_line = image->comps[compno].data + y * image->comps[compno].w;

            for (x = 0; x < image->comps[compno].w; ++x) {

                image_line[x] = image_line[x - image->comps[compno].w];

            }

        }

    }



    return 1;

}
