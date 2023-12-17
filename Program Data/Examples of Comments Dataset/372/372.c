/*
This function is responsible for decoding video frames using the given AVCodecContext and AVPacket in the FFmpeg library.

Here's the detail:
- The function first checks the coded width and height of the video frames using av_image_check_size to ensure they are valid. If not, it returns an error.
- It then sets the packet of the codec context to the provided AVPacket and applies any parameter changes using the apply_param_change function.
- Depending on the codec capabilities, packet size, and active thread type, the function either utilizes multi-threaded decoding with ff_thread_decode_frame or single-threaded decoding with avctx->codec->decode.
- It sets various properties of the decoded picture such as packet DTS, sample aspect ratio, width, height, and pixel format.
- Finally, it ensures that the extended_data of the picture is correctly set before returning the result of the decoding operation.

Needs to notice:
- The function handles both multi-threaded and single-threaded decoding based on codec capabilities and active thread type.
- Decoders may assign whole AVFrames, potentially overwriting extended_data. This function takes care to set it correctly after the decoding operation.
- Proper error handling and resource management should be ensured when using this function, especially when working with multi-threaded decoding or when making changes to AVCodecContext and AVPacket before calling this function.
*/
int attribute_align_arg avcodec_decode_video2(AVCodecContext *avctx, AVFrame *picture,
                                              int *got_picture_ptr,
                                              AVPacket *avpkt)
{
    int ret;
    *got_picture_ptr = 0;
    if ((avctx->coded_width || avctx->coded_height) && av_image_check_size(avctx->coded_width, avctx->coded_height, 0, avctx))
        return -1;
    avctx->pkt = avpkt;
    apply_param_change(avctx, avpkt);
    if ((avctx->codec->capabilities & CODEC_CAP_DELAY) || avpkt->size || (avctx->active_thread_type & FF_THREAD_FRAME)) {
        if (HAVE_THREADS && avctx->active_thread_type & FF_THREAD_FRAME)
            ret = ff_thread_decode_frame(avctx, picture, got_picture_ptr,
                                         avpkt);
        else {
            ret = avctx->codec->decode(avctx, picture, got_picture_ptr,
                                       avpkt);
            picture->pkt_dts             = avpkt->dts;
            picture->sample_aspect_ratio = avctx->sample_aspect_ratio;
            picture->width               = avctx->width;
            picture->height              = avctx->height;
            picture->format              = avctx->pix_fmt;
        }
        emms_c(); //needed to avoid an emms_c() call before every return;
        if (*got_picture_ptr)
            avctx->frame_number++;
    } else
        ret = 0;
    /* many decoders assign whole AVFrames, thus overwriting extended_data;
     * make sure it's set correctly */
    picture->extended_data = picture->data;
    return ret;
}