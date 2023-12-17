/*
This function is responsible for encoding a video frame using the FFV1 video codec.

Here's the detail: The function first initializes a context for Context-Adaptive Binary Arithmetic Coding (CABAC) and initializes the encoding process. It sets the encoding parameters based on the input frame, such as width, height, and picture type. Depending on the GOP (Group of Pictures) size, it marks the frame as a key frame or a non-key frame and appropriately writes the header and clears the state. It then proceeds to encode the pixel data for the frame, considering color space and chroma subsampling if applicable. After encoding, it finalizes the encoding process and prepares the output bitstream for storage or transmission.

Need's to notice: It's important to ensure that the AVCodecContext and the FFV1Context are properly initialized and configured before invoking this function for encoding. Additionally, the function includes a warning message regarding the development status of the codec and its compatibility with future versions, suggesting a workaround using vstrict=-1. This warning should be considered when using this codec for production purposes or when planning for future compatibility. Lastly, the use of the CABAC encoding scheme and its associated parameters such as lps_state should be carefully reviewed and validated to ensure the correctness of the encoding process.
*/
static int encode_frame(AVCodecContext *avctx, unsigned char *buf, int buf_size, void *data){

    FFV1Context *f = avctx->priv_data;

    CABACContext * const c= &f->c;

    AVFrame *pict = data;

    const int width= f->width;

    const int height= f->height;

    AVFrame * const p= &f->picture;

    int used_count= 0;



    if(avctx->strict_std_compliance >= 0){

        av_log(avctx, AV_LOG_ERROR, "this codec is under development, files encoded with it wont be decodeable with future versions!!!\n"

               "use vstrict=-1 to use it anyway\n");

        return -1;

    }

        

    ff_init_cabac_encoder(c, buf, buf_size);

    ff_init_cabac_states(c, ff_h264_lps_range, ff_h264_mps_state, ff_h264_lps_state, 64);

    c->lps_state[2] = 1;

    c->lps_state[3] = 0;

    

    *p = *pict;

    p->pict_type= FF_I_TYPE;

    

    if(avctx->gop_size==0 || f->picture_number % avctx->gop_size == 0){

        put_cabac_bypass(c, 1);

        p->key_frame= 1;

        write_header(f);

        clear_state(f);

    }else{

        put_cabac_bypass(c, 0);

        p->key_frame= 0;

    }



    if(!f->ac){

        used_count += put_cabac_terminate(c, 1);

//printf("pos=%d\n", used_count);

        init_put_bits(&f->pb, buf + used_count, buf_size - used_count);

    }

    

    if(f->colorspace==0){

        const int chroma_width = -((-width )>>f->chroma_h_shift);

        const int chroma_height= -((-height)>>f->chroma_v_shift);



        encode_plane(f, p->data[0], width, height, p->linesize[0], 0);



        encode_plane(f, p->data[1], chroma_width, chroma_height, p->linesize[1], 1);

        encode_plane(f, p->data[2], chroma_width, chroma_height, p->linesize[2], 1);

    }else{

        encode_rgb_frame(f, (uint32_t*)(p->data[0]), width, height, p->linesize[0]/4);

    }

    emms_c();

    

    f->picture_number++;



    if(f->ac){

        return put_cabac_terminate(c, 1);

    }else{

        flush_put_bits(&f->pb); //nicer padding FIXME

        return used_count + (put_bits_count(&f->pb)+7)/8;

    }

}
