/*
This function is responsible for calculating the rematrixing strategy for AC3 audio encoding. It iterates through the audio blocks and analyzes the coefficients to determine whether rematrixing should be used for different frequency bands within each block.

Here's the detail:
The function first sets the number of rematrixing bands to 4 and checks if the rematrixing strategy is set to static. If it is, the function returns early without further computation. It then calculates the number of coefficients to consider based on the minimum coefficient count between the left and right audio channels.

For each audio block, the function computes the new rematrixing strategy by analyzing the sum of squared coefficients for each frequency band within the block. It then compares the sums to decide whether rematrixing should be applied to each band. If there is a change in the rematrixing flags compared to the previous block, the new rematrixing strategy flag is set accordingly.

Need's to notice:
It's important to understand the significance of the rematrixing strategy and the impact it has on the encoding process. Additionally, the specifics of the coefficients and their manipulation play a crucial role in determining the rematrixing flags for each frequency band.
*/
static void compute_rematrixing_strategy(AC3EncodeContext *s)

{

    int nb_coefs;

    int blk, bnd, i;

    AC3Block *block, *block0;



    s->num_rematrixing_bands = 4;



    if (s->rematrixing & AC3_REMATRIXING_IS_STATIC)

        return;



    nb_coefs = FFMIN(s->nb_coefs[0], s->nb_coefs[1]);



    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {

        block = &s->blocks[blk];

        block->new_rematrixing_strategy = !blk;

        for (bnd = 0; bnd < s->num_rematrixing_bands; bnd++) {

            /* calculate calculate sum of squared coeffs for one band in one block */

            int start = ff_ac3_rematrix_band_tab[bnd];

            int end   = FFMIN(nb_coefs, ff_ac3_rematrix_band_tab[bnd+1]);

            CoefSumType sum[4] = {0,};

            for (i = start; i < end; i++) {

                CoefType lt = block->mdct_coef[0][i];

                CoefType rt = block->mdct_coef[1][i];

                CoefType md = lt + rt;

                CoefType sd = lt - rt;

                sum[0] += lt * lt;

                sum[1] += rt * rt;

                sum[2] += md * md;

                sum[3] += sd * sd;

            }



            /* compare sums to determine if rematrixing will be used for this band */

            if (FFMIN(sum[2], sum[3]) < FFMIN(sum[0], sum[1]))

                block->rematrixing_flags[bnd] = 1;

            else

                block->rematrixing_flags[bnd] = 0;



            /* determine if new rematrixing flags will be sent */

            if (blk &&

                block->rematrixing_flags[bnd] != block0->rematrixing_flags[bnd]) {

                block->new_rematrixing_strategy = 1;

            }

        }

        block0 = block;

    }

}
