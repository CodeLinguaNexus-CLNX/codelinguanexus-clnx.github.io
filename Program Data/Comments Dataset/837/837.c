/*
This function is responsible for handling the writing of data for a SCSI request to a disk device.

Here's the detail of the function:
- The function begins by extracting the SCSIDiskReq and SCSIDiskState structures from the given SCSI request.
- It then performs various checks and operations based on the type and parameters of the SCSI request:
  - It asserts that no data transfer is already in progress for the request and adds a reference to the request as an AIO opaque value.
  - It checks the data transfer direction and handles cases where the direction is invalid by logging a debug message and completing the write operation with an error.
  - If no scatter-gather list (sg) and no data buffer size are provided, it signals the driver to send more data and completes the write operation.
  - It checks if the disk tray is open and completes the write operation with an error if so.
  - It handles VERIFY commands separately by checking for the presence of a scatter-gather list and completing the data transfer accordingly.
  - For regular write operations, it initiates the actual data transfer:
    - If a scatter-gather list is provided, it starts the DMA accounting, updates the residual data size, and initiates asynchronous I/O operation for writing using the scatter-gather list.
    - If only a data vector (qiov) is provided, it starts the block device accounting, calculates the number of sectors to write, and initiates asynchronous I/O operation for writing using the data vector.

Needs to notice:
- Understanding the SCSI protocol and the specifics of data writing for SCSI requests is crucial for correctly handling the various types of requests and their parameters.
- Proper error handling and completion of the write operation are necessary for maintaining the integrity and consistency of data on the disk device.
- Careful consideration of different transfer modes and scenarios, such as tray open status and VERIFY commands, is essential for ensuring the correctness and efficiency of data writing operations.
- Attention to proper resource management, such as reference counting and asynchronous I/O operations, is important for the reliable and efficient execution of the data writing process.
*/
static void scsi_write_data(SCSIRequest *req)

{

    SCSIDiskReq *r = DO_UPCAST(SCSIDiskReq, req, req);

    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, r->req.dev);

    uint32_t n;



    /* No data transfer may already be in progress */

    assert(r->req.aiocb == NULL);



    /* The request is used as the AIO opaque value, so add a ref.  */

    scsi_req_ref(&r->req);

    if (r->req.cmd.mode != SCSI_XFER_TO_DEV) {

        DPRINTF("Data transfer direction invalid\n");

        scsi_write_complete(r, -EINVAL);

        return;

    }



    if (!r->req.sg && !r->qiov.size) {

        /* Called for the first time.  Ask the driver to send us more data.  */

        r->started = true;

        scsi_write_complete(r, 0);

        return;

    }

    if (s->tray_open) {

        scsi_write_complete(r, -ENOMEDIUM);

        return;

    }



    if (r->req.cmd.buf[0] == VERIFY_10 || r->req.cmd.buf[0] == VERIFY_12 ||

        r->req.cmd.buf[0] == VERIFY_16) {

        if (r->req.sg) {

            scsi_dma_complete(r, 0);

        } else {

            scsi_write_complete(r, 0);

        }

        return;

    }



    if (r->req.sg) {

        dma_acct_start(s->qdev.conf.bs, &r->acct, r->req.sg, BDRV_ACCT_WRITE);

        r->req.resid -= r->req.sg->size;

        r->req.aiocb = dma_bdrv_write(s->qdev.conf.bs, r->req.sg, r->sector,

                                      scsi_dma_complete, r);

    } else {

        n = r->qiov.size / 512;

        bdrv_acct_start(s->qdev.conf.bs, &r->acct, n * BDRV_SECTOR_SIZE, BDRV_ACCT_WRITE);

        r->req.aiocb = bdrv_aio_writev(s->qdev.conf.bs, r->sector, &r->qiov, n,

                                       scsi_write_complete, r);

    }

}
