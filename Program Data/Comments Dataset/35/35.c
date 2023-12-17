/*
This function is mainly responsible for calculating the exact subframe count needed to connect the Xen 9pfs device.
Here¡¯s the detail:
Specifically, the function first accumulates the 8-byte size of the subframe header.
Then, the sum is calculated according to different subframe types. If the subframe type is FLAC_SUBFRAME_CONSTANT, the sub->obits size is accumulated; If the subframe type is FLAC_SUBFRAME_VERBATIM, the cumulative size is s->frame.blocksize multiplied by sub->obits.
For other types of subframes, an accumulation calculation is performed based on the prediction order and different parts of the subframe. The first is the warm-up sample part, which accumulates the prediction order multiplied by the value of sub->obits size. Then there are the LPC coefficients, which, if the subframe type is FLAC_SUBFRAME_LPC, add up to 4 + 5 + pred_order * s->options.lpc_coeff_precision; Then, the rice-encoded block part accumulated 2 values. Finally, the residual part is calculated according to the number of different partitions.
Need¡¯s to notice:
When using this function, you need to be careful that the arguments you pass in are valid, especially for pointer arguments, to ensure that the memory space the pointer points to is valid to avoid dangling Pointers or memory access errors. You also need to be careful how you handle the results of the computations to make sure you don't overflow or other numerical errors.
*/
static int subframe_count_exact(FlacEncodeContext *s, FlacSubframe *sub,

                                int pred_order)

{

    int p, porder, psize;

    int i, part_end;

    int count = 0;



    /* subframe header */

    count += 8;



    /* subframe */

    if (sub->type == FLAC_SUBFRAME_CONSTANT) {

        count += sub->obits;

    } else if (sub->type == FLAC_SUBFRAME_VERBATIM) {

        count += s->frame.blocksize * sub->obits;

    } else {

        /* warm-up samples */

        count += pred_order * sub->obits;



        /* LPC coefficients */

        if (sub->type == FLAC_SUBFRAME_LPC)

            count += 4 + 5 + pred_order * s->options.lpc_coeff_precision;



        /* rice-encoded block */

        count += 2;



        /* partition order */

        porder = sub->rc.porder;

        psize  = s->frame.blocksize >> porder;

        count += 4;



        /* residual */

        i        = pred_order;

        part_end = psize;

        for (p = 0; p < 1 << porder; p++) {

            int k = sub->rc.params[p];

            count += 4;

            count += rice_count_exact(&sub->residual[i], part_end - i, k);

            i = part_end;

            part_end = FFMIN(s->frame.blocksize, part_end + psize);

        }

    }



    return count;

}
