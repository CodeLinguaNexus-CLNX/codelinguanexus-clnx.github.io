/*
This function is responsible for releasing resources and performing cleanup operations at the end of the PNG decoding process in an FFmpeg-based application.

Here's the detail:
- The function releases the thread buffers for the previous picture, last picture, and the main picture being decoded. It also frees the associated frame structures.
- It frees the buffer used for decoding, as well as the memory allocated for the last row and the temporary row used during decoding.
- Additionally, it frees any extra data associated with the PNG decoding.

Needs to notice:
- Care should be taken to ensure that all allocated resources are properly released to avoid memory leaks.
- The function should be called at the appropriate point in the decoding process to ensure proper cleanup and resource management.
- It's important to verify that the cleanup operations are comprehensive and cover all dynamically allocated resources to prevent memory leaks or resource waste.
*/
static av_cold int png_dec_end(AVCodecContext *avctx)

{

    PNGDecContext *s = avctx->priv_data;



    ff_thread_release_buffer(avctx, &s->previous_picture);

    av_frame_free(&s->previous_picture.f);

    ff_thread_release_buffer(avctx, &s->last_picture);

    av_frame_free(&s->last_picture.f);

    ff_thread_release_buffer(avctx, &s->picture);

    av_frame_free(&s->picture.f);

    av_freep(&s->buffer);

    s->buffer_size = 0;

    av_freep(&s->last_row);

    s->last_row_size = 0;

    av_freep(&s->tmp_row);

    s->tmp_row_size = 0;

    av_freep(&s->extra_data);

    s->extra_data_size = 0;



    return 0;

}
