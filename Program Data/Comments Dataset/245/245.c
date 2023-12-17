/*
This function is responsible for reading and processing H.265/HEVC NAL (Network Abstraction Layer) units in a coded bitstream. It identifies the type of NAL unit and performs specific actions based on the unit type, such as reading VPS (Video Parameter Set), SPS (Sequence Parameter Set), PPS (Picture Parameter Set), slice headers, AUD (Access Unit Delimiter), and handling other NAL unit types as per the H.265/HEVC standard.

Here's the detail:
1. The function initializes a BitstreamContext for the given CodedBitstreamUnit data and size.
2. Depending on the type of NAL unit (e.g., VPS, SPS, PPS, slice headers, AUD, etc.), the function dynamically allocates memory for the corresponding H.265 raw structure (e.g., H265RawVPS, H265RawSPS, H265RawPPS, H265RawSlice, H265RawAUD).
3. It then calls specific functions (e.g., cbs_h265_read_vps, cbs_h265_read_sps, cbs_h265_read_pps, cbs_h265_read_slice_segment_header, cbs_h265_read_aud) to read and populate the allocated structures with the parsed data from the bitstream context.
4. Error handling is performed for memory allocation failures and read errors, ensuring the proper cleanup and error propagation.

Need's to notice:
1. The function assumes that the input CodedBitstreamUnit contains valid H.265/HEVC NAL unit data and type.
2. Proper memory management and error handling are crucial, as the function dynamically allocates memory for various H.265 raw structures.
3. The correct behavior and error handling of the called functions (e.g., cbs_h265_read_vps, cbs_h265_read_sps, etc.) should be ensured and tested thoroughly.
4. The handling of different NAL unit types and their impact on the subsequent processing should be considered in the broader context of H.265/HEVC bitstream parsing and decoding.

*/
static int cbs_h265_read_nal_unit(CodedBitstreamContext *ctx,

                                  CodedBitstreamUnit *unit)

{

    BitstreamContext bc;

    int err;



    err = bitstream_init(&bc, unit->data, 8 * unit->data_size);

    if (err < 0)

        return err;



    switch (unit->type) {

    case HEVC_NAL_VPS:

        {

            H265RawVPS *vps;



            vps = av_mallocz(sizeof(*vps));

            if (!vps)

                return AVERROR(ENOMEM);

            err = cbs_h265_read_vps(ctx, &bc, vps);

            if (err >= 0)

                err = cbs_h265_replace_vps(ctx, vps);

            if (err < 0) {

                av_free(vps);

                return err;

            }



            unit->content = vps;

        }

        break;

    case HEVC_NAL_SPS:

        {

            H265RawSPS *sps;



            sps = av_mallocz(sizeof(*sps));

            if (!sps)

                return AVERROR(ENOMEM);

            err = cbs_h265_read_sps(ctx, &bc, sps);

            if (err >= 0)

                err = cbs_h265_replace_sps(ctx, sps);

            if (err < 0) {

                av_free(sps);

                return err;

            }



            unit->content = sps;

        }

        break;



    case HEVC_NAL_PPS:

        {

            H265RawPPS *pps;



            pps = av_mallocz(sizeof(*pps));

            if (!pps)

                return AVERROR(ENOMEM);

            err = cbs_h265_read_pps(ctx, &bc, pps);

            if (err >= 0)

                err = cbs_h265_replace_pps(ctx, pps);

            if (err < 0) {

                av_free(pps);

                return err;

            }



            unit->content = pps;

        }

        break;



    case HEVC_NAL_TRAIL_N:

    case HEVC_NAL_TRAIL_R:

    case HEVC_NAL_TSA_N:

    case HEVC_NAL_TSA_R:

    case HEVC_NAL_STSA_N:

    case HEVC_NAL_STSA_R:

    case HEVC_NAL_RADL_N:

    case HEVC_NAL_RADL_R:

    case HEVC_NAL_RASL_N:

    case HEVC_NAL_RASL_R:

    case HEVC_NAL_BLA_W_LP:

    case HEVC_NAL_BLA_W_RADL:

    case HEVC_NAL_BLA_N_LP:

    case HEVC_NAL_IDR_W_RADL:

    case HEVC_NAL_IDR_N_LP:

    case HEVC_NAL_CRA_NUT:

        {

            H265RawSlice *slice;

            int pos, len;



            slice = av_mallocz(sizeof(*slice));

            if (!slice)

                return AVERROR(ENOMEM);

            err = cbs_h265_read_slice_segment_header(ctx, &bc, &slice->header);

            if (err < 0) {

                av_free(slice);

                return err;

            }



            pos = bitstream_tell(&bc);

            len = unit->data_size;

            if (!unit->data[len - 1]) {

                int z;

                for (z = 0; z < len && !unit->data[len - z - 1]; z++);

                av_log(ctx->log_ctx, AV_LOG_DEBUG, "Deleted %d trailing zeroes "

                       "from slice data.\n", z);

                len -= z;

            }



            slice->data_size = len - pos / 8;

            slice->data = av_malloc(slice->data_size);

            if (!slice->data) {

                av_free(slice);

                return AVERROR(ENOMEM);

            }

            memcpy(slice->data,

                   unit->data + pos / 8, slice->data_size);

            slice->data_bit_start = pos % 8;



            unit->content = slice;

        }

        break;



    case HEVC_NAL_AUD:

        {

            H265RawAUD *aud;



            aud = av_mallocz(sizeof(*aud));

            if (!aud)

                return AVERROR(ENOMEM);

            err = cbs_h265_read_aud(ctx, &bc, aud);

            if (err < 0) {

                av_free(aud);

                return err;

            }



            unit->content = aud;

        }

        break;



    default:

        return AVERROR(ENOSYS);

    }



    return 0;

}
