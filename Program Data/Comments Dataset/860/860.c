/*
This function is responsible for updating the block limits for an iSCSI block device based on the characteristics of the underlying iSCSI LUN (Logical Unit Number).

Here's the detail of the function:
- The function initializes the parameters for the block limits of the iSCSI block device, such as request alignment, maximum transfer length, discard alignment, maximum write same length, and optimal transfer length, based on the properties of the associated iSCSI LUN.
- It uses the information from the IscsiLun structure referenced by bs->opaque to determine the characteristics and parameters of the iSCSI LUN, such as the block size, maximum transfer length, maximum unmap size, discard alignment, maximum write same length, and optimal transfer length.
- The function then calculates and sets the block limits for the iSCSI block device based on the properties of the iSCSI LUN, ensuring that the block device adheres to the constraints and capabilities of the underlying storage.

Needs to notice:
- Understanding the iSCSI protocol and the structure of iSCSI LUN parameters is crucial for correctly updating the block limits of the iSCSI block device.
- Careful consideration should be given to the calculation and setting of block limits to ensure that they are within the valid range and adhere to the specifications of the iSCSI protocol.
- Proper error handling and reporting should be implemented to handle any failures or discrepancies in retrieving the iSCSI LUN parameters and updating the block limits accordingly.
- Ensuring the consistency and compatibility of block limits with the iSCSI LUN properties is important for the correct functioning and performance of the iSCSI block device.
*/
static void iscsi_refresh_limits(BlockDriverState *bs, Error **errp)

{

    /* We don't actually refresh here, but just return data queried in

     * iscsi_open(): iscsi targets don't change their limits. */



    IscsiLun *iscsilun = bs->opaque;

    uint64_t max_xfer_len = iscsilun->use_16_for_rw ? 0xffffffff : 0xffff;



    bs->bl.request_alignment = iscsilun->block_size;



    if (iscsilun->bl.max_xfer_len) {

        max_xfer_len = MIN(max_xfer_len, iscsilun->bl.max_xfer_len);

    }



    if (max_xfer_len * iscsilun->block_size < INT_MAX) {

        bs->bl.max_transfer = max_xfer_len * iscsilun->block_size;

    }



    if (iscsilun->lbp.lbpu) {

        if (iscsilun->bl.max_unmap < 0xffffffff / iscsilun->block_size) {

            bs->bl.max_pdiscard =

                iscsilun->bl.max_unmap * iscsilun->block_size;

        }

        bs->bl.pdiscard_alignment =

            iscsilun->bl.opt_unmap_gran * iscsilun->block_size;

    } else {

        bs->bl.pdiscard_alignment = iscsilun->block_size;

    }



    if (iscsilun->bl.max_ws_len < 0xffffffff / iscsilun->block_size) {

        bs->bl.max_pwrite_zeroes =

            iscsilun->bl.max_ws_len * iscsilun->block_size;

    }

    if (iscsilun->lbp.lbpws) {

        bs->bl.pwrite_zeroes_alignment =

            iscsilun->bl.opt_unmap_gran * iscsilun->block_size;

    } else {

        bs->bl.pwrite_zeroes_alignment = iscsilun->block_size;

    }

    if (iscsilun->bl.opt_xfer_len &&

        iscsilun->bl.opt_xfer_len < INT_MAX / iscsilun->block_size) {

        bs->bl.opt_transfer = pow2floor(iscsilun->bl.opt_xfer_len *

                                        iscsilun->block_size);

    }

}
