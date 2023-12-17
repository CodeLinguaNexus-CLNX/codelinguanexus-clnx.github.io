/*
This function is responsible for performing asynchronous write operations on a block device using the Sheepdog storage backend, while also handling potential extension of the underlying storage if the block device is growable.

Here's the detail of the function:
- It starts by checking if the block device is growable and if the write operation would exceed the current total number of sectors in the block device. If so, it calls the sd_truncate function to extend the storage to accommodate the additional sectors.
- Upon successful extension of the storage, it updates the total_sectors field of the block device's structure to reflect the new total sector count.
- It then sets up a SheepdogAIOCB (Asynchronous I/O Control Block) for the write operation using the sd_aio_setup function, providing the block device, I/O vector, sector number, and number of sectors as parameters.
- The function configures the asynchronous I/O control block by setting the aio_done_func to sd_write_done and aiocb_type to AIOCB_WRITE_UDATA.
- The actual read/write operation is performed asynchronously through the sd_co_rw_vector function, and the function checks the returned value to handle the completion or error scenarios.
- If the return value indicates completion or an error, the function releases the asynchronous I/O control block using qemu_aio_release and returns the appropriate value.

Needs to notice:
- This function relies on the correct implementation and behavior of the underlying Sheepdog storage backend and asynchronous I/O handling mechanisms.
- Proper error handling and recovery procedures should be in place to handle various failure scenarios, such as storage extension failures or asynchronous I/O operation failures.
- Understanding the specifics of the Sheepdog storage backend and its interaction with the block device layer is essential for effectively using and modifying this function.
*/
static coroutine_fn int sd_co_writev(BlockDriverState *bs, int64_t sector_num,

                        int nb_sectors, QEMUIOVector *qiov)

{

    SheepdogAIOCB *acb;

    int ret;



    if (bs->growable && sector_num + nb_sectors > bs->total_sectors) {

        ret = sd_truncate(bs, (sector_num + nb_sectors) * BDRV_SECTOR_SIZE);

        if (ret < 0) {

            return ret;

        }

        bs->total_sectors = sector_num + nb_sectors;

    }



    acb = sd_aio_setup(bs, qiov, sector_num, nb_sectors);

    acb->aio_done_func = sd_write_done;

    acb->aiocb_type = AIOCB_WRITE_UDATA;



    ret = sd_co_rw_vector(acb);

    if (ret <= 0) {

        qemu_aio_release(acb);

        return ret;

    }



    qemu_coroutine_yield();



    return acb->ret;

}
