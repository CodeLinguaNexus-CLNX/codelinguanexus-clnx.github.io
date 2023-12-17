/*
This function is responsible for handling the put operation for Translation Control Entries (TCEs) within the context of the sPAPR (Shared Power Architecture Platform Reference) environment.

Here's the detail:
The function takes in the necessary parameters for the TCE put operation, including the Logical I/O Bus Number (liobn), the I/O Bus Address (ioba), and the TCE value. It then locates the corresponding VIOsPAPRDevice based on the liobn and performs the TCE update operation after performing necessary address and bounds checking.

Needs to notice:
1. The function operates within the sPAPR environment and is likely crucial for managing I/O and memory-mapped operations within the virtualized environment.
2. Proper bounds checking and error handling are performed to ensure the integrity of the TCE update operation.
3. It's important to verify the input parameters and their validity within the context of the sPAPR environment to avoid potential security or integrity issues.
*/
static target_ulong h_put_tce(CPUPPCState *env, sPAPREnvironment *spapr,

                              target_ulong opcode, target_ulong *args)

{

    target_ulong liobn = args[0];

    target_ulong ioba = args[1];

    target_ulong tce = args[2];

    VIOsPAPRDevice *dev = spapr_vio_find_by_reg(spapr->vio_bus, liobn);

    VIOsPAPR_RTCE *rtce;



    if (!dev) {

        hcall_dprintf("LIOBN 0x" TARGET_FMT_lx " does not exist\n", liobn);

        return H_PARAMETER;

    }



    ioba &= ~(SPAPR_VIO_TCE_PAGE_SIZE - 1);



#ifdef DEBUG_TCE

    fprintf(stderr, "spapr_vio_put_tce on %s  ioba 0x" TARGET_FMT_lx

            "  TCE 0x" TARGET_FMT_lx "\n", dev->qdev.id, ioba, tce);

#endif



    if (ioba >= dev->rtce_window_size) {

        hcall_dprintf("Out-of-bounds IOBA 0x" TARGET_FMT_lx "\n", ioba);

        return H_PARAMETER;

    }



    rtce = dev->rtce_table + (ioba >> SPAPR_VIO_TCE_PAGE_SHIFT);

    rtce->tce = tce;



    return H_SUCCESS;

}
