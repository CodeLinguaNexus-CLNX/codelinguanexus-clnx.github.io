/*
This function is responsible for copying an interrupt response block (IRB) from a source to a destination in a guest configuration, considering specific control conditions.

Here's the detail:
- The function begins by extracting certain control parameters from the source IRB, such as stctl and actl, using bitwise operations.
- It then proceeds to copy various fields from the source IRB to the destination IRB, including scsw, esw, and ecw arrays, after performing endian conversion as necessary.
- Additionally, the function checks for certain flags and conditions related to extended measurements and specific control statuses, and copies the emw array from the source IRB to the destination in certain specific cases.

Usage needs to notice:
- This function is intricately tied to the control and measurement aspects of an interrupt response block, and any modifications should be made with deep understanding of these concepts.
- Developers should ensure that the endian conversion is performed correctly to avoid issues related to endianness when copying the data.
- It's crucial to have a clear understanding of the expected behavior of the extended measurements, status control, and action control flags to ensure correct handling of the IRB copying process.
*/
static void copy_irb_to_guest(IRB *dest, const IRB *src, PMCW *pmcw)

{

    int i;

    uint16_t stctl = src->scsw.ctrl & SCSW_CTRL_MASK_STCTL;

    uint16_t actl = src->scsw.ctrl & SCSW_CTRL_MASK_ACTL;



    copy_scsw_to_guest(&dest->scsw, &src->scsw);



    for (i = 0; i < ARRAY_SIZE(dest->esw); i++) {

        dest->esw[i] = cpu_to_be32(src->esw[i]);

    }

    for (i = 0; i < ARRAY_SIZE(dest->ecw); i++) {

        dest->ecw[i] = cpu_to_be32(src->ecw[i]);

    }

    /* extended measurements enabled? */

    if ((src->scsw.flags & SCSW_FLAGS_MASK_ESWF) ||

        !(pmcw->flags & PMCW_FLAGS_MASK_TF) ||

        !(pmcw->chars & PMCW_CHARS_MASK_XMWME)) {

        return;

    }

    /* extended measurements pending? */

    if (!(stctl & SCSW_STCTL_STATUS_PEND)) {

        return;

    }

    if ((stctl & SCSW_STCTL_PRIMARY) ||

        (stctl == SCSW_STCTL_SECONDARY) ||

        ((stctl & SCSW_STCTL_INTERMEDIATE) && (actl & SCSW_ACTL_SUSP))) {

        for (i = 0; i < ARRAY_SIZE(dest->emw); i++) {

            dest->emw[i] = cpu_to_be32(src->emw[i]);

        }

    }

}
