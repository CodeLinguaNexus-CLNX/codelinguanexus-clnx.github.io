/*
This function is responsible for decoding a channel sound unit in an ATRAC3 audio stream.

Here's the detail:
- It takes pointers to an ATRAC3Context structure, a GetBitContext structure, a channel_unit structure, an array of floats for output, as well as integer parameters for the channel number and coding mode.
- It performs validation checks based on the coding mode and channel number, logging an error and returning -1 if the checks fail.
- It reads the number of coded QMF bands from the bitstream and decodes the gain control and tonal components based on the received data.
- It then decodes the spectrum and tonal components, merging them together.
- The function calculates the number of used MLT/QMF bands based on the coded spectral lines and performs the reconstruction of time domain samples using IMDCT and gain compensation and overlapping techniques.
- Finally, it swaps the gain control buffers for the next frame and returns 0 to indicate successful decoding.

Needs to notice:
- The function assumes that the ATRAC3Context, GetBitContext, and channel_unit structures are properly initialized and managed before being passed to this function.
- Proper error handling and recovery mechanisms should be considered when using this function in a larger audio decoding context.
- Understanding the ATRAC3 audio codec and related data structures is important for fully comprehending this decoding process.
*/
static int decodeChannelSoundUnit (ATRAC3Context *q, GetBitContext *gb, channel_unit *pSnd, float *pOut, int channelNum, int codingMode)

{

    int   band, result=0, numSubbands, lastTonal, numBands;



    if (codingMode == JOINT_STEREO && channelNum == 1) {

        if (get_bits(gb,2) != 3) {

            av_log(NULL,AV_LOG_ERROR,"JS mono Sound Unit id != 3.\n");

            return -1;

        }

    } else {

        if (get_bits(gb,6) != 0x28) {

            av_log(NULL,AV_LOG_ERROR,"Sound Unit id != 0x28.\n");

            return -1;

        }

    }



    /* number of coded QMF bands */

    pSnd->bandsCoded = get_bits(gb,2);



    result = decodeGainControl (gb, &(pSnd->gainBlock[pSnd->gcBlkSwitch]), pSnd->bandsCoded);

    if (result) return result;



    pSnd->numComponents = decodeTonalComponents (gb, pSnd->components, pSnd->bandsCoded);

    if (pSnd->numComponents == -1) return -1;



    numSubbands = decodeSpectrum (gb, pSnd->spectrum);



    /* Merge the decoded spectrum and tonal components. */

    lastTonal = addTonalComponents (pSnd->spectrum, pSnd->numComponents, pSnd->components);





    /* calculate number of used MLT/QMF bands according to the amount of coded spectral lines */

    numBands = (subbandTab[numSubbands] - 1) >> 8;

    if (lastTonal >= 0)

        numBands = FFMAX((lastTonal + 256) >> 8, numBands);





    /* Reconstruct time domain samples. */

    for (band=0; band<4; band++) {

        /* Perform the IMDCT step without overlapping. */

        if (band <= numBands) {

            IMLT(&(pSnd->spectrum[band*256]), pSnd->IMDCT_buf, band&1);

        } else

            memset(pSnd->IMDCT_buf, 0, 512 * sizeof(float));



        /* gain compensation and overlapping */

        gainCompensateAndOverlap (pSnd->IMDCT_buf, &(pSnd->prevFrame[band*256]), &(pOut[band*256]),

                                    &((pSnd->gainBlock[1 - (pSnd->gcBlkSwitch)]).gBlock[band]),

                                    &((pSnd->gainBlock[pSnd->gcBlkSwitch]).gBlock[band]));

    }



    /* Swap the gain control buffers for the next frame. */

    pSnd->gcBlkSwitch ^= 1;



    return 0;

}
