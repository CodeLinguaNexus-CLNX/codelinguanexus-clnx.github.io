/*
This function is responsible for creating a snapshot of a Sheepdog virtual disk image (VDI) by updating the SheepdogInode structure and performing necessary operations on the underlying Sheepdog storage cluster.

Here's the detail of the function:
- The function first checks if the VDI is already a snapshot by examining the is_snapshot flag in the BDRVSheepdogState structure. If it is a snapshot, an error is reported, and the function returns with an error code.
- Upon confirming that the VDI is not a snapshot, the function proceeds to update the metadata of the VDI based on the provided QEMUSnapshotInfo, including the snapshot name, size, clock information, and tag.
- The function connects to the Sheepdog storage cluster, writes the updated inode object, creates a new inode for the snapshot, reads the information of the new inode, and updates the VDI's inode with the new snapshot information.
- Finally, the function handles cleanup by closing the socket and returning the result of the operation.

Needs to notice:
- The function heavily interacts with the Sheepdog storage cluster, including connecting, writing, creating, and reading objects. Proper error handling and recovery mechanisms are crucial to ensure data integrity and prevent inconsistencies in the storage cluster.
- Memory management for the inode structure allocated using g_malloc should be carefully handled to prevent memory leaks and ensure proper deallocation after its use.
- Error reporting and logging should be detailed and informative to aid in diagnosing any issues that may arise during the snapshot creation process.
- Network and I/O operations within the function should be implemented with considerations for potential timeouts, retries, and resilience to network or cluster-related failures.
- The function's behavior and potential side effects regarding the manipulation of Sheepdog VDIs and snapshots should be thoroughly documented to ensure correct usage and understanding by developers and users.
*/
static int sd_snapshot_create(BlockDriverState *bs, QEMUSnapshotInfo *sn_info)

{

    BDRVSheepdogState *s = bs->opaque;

    int ret, fd;

    uint32_t new_vid;

    SheepdogInode *inode;

    unsigned int datalen;



    dprintf("sn_info: name %s id_str %s s: name %s vm_state_size %" PRId64 " "

            "is_snapshot %d\n", sn_info->name, sn_info->id_str,

            s->name, sn_info->vm_state_size, s->is_snapshot);



    if (s->is_snapshot) {

        error_report("You can't create a snapshot of a snapshot VDI, "

                     "%s (%" PRIu32 ").", s->name, s->inode.vdi_id);



        return -EINVAL;

    }



    dprintf("%s %s\n", sn_info->name, sn_info->id_str);



    s->inode.vm_state_size = sn_info->vm_state_size;

    s->inode.vm_clock_nsec = sn_info->vm_clock_nsec;

    /* It appears that inode.tag does not require a NUL terminator,

     * which means this use of strncpy is ok.

     */

    strncpy(s->inode.tag, sn_info->name, sizeof(s->inode.tag));

    /* we don't need to update entire object */

    datalen = SD_INODE_SIZE - sizeof(s->inode.data_vdi_id);



    /* refresh inode. */

    fd = connect_to_sdog(s->addr, s->port);

    if (fd < 0) {

        ret = fd;

        goto cleanup;

    }



    ret = write_object(fd, (char *)&s->inode, vid_to_vdi_oid(s->inode.vdi_id),

                       s->inode.nr_copies, datalen, 0, false, s->cache_enabled);

    if (ret < 0) {

        error_report("failed to write snapshot's inode.");

        goto cleanup;

    }



    ret = do_sd_create(s->name, s->inode.vdi_size, s->inode.vdi_id, &new_vid, 1,

                       s->addr, s->port);

    if (ret < 0) {

        error_report("failed to create inode for snapshot. %s",

                     strerror(errno));

        goto cleanup;

    }



    inode = (SheepdogInode *)g_malloc(datalen);



    ret = read_object(fd, (char *)inode, vid_to_vdi_oid(new_vid),

                      s->inode.nr_copies, datalen, 0, s->cache_enabled);



    if (ret < 0) {

        error_report("failed to read new inode info. %s", strerror(errno));

        goto cleanup;

    }



    memcpy(&s->inode, inode, datalen);

    dprintf("s->inode: name %s snap_id %x oid %x\n",

            s->inode.name, s->inode.snap_id, s->inode.vdi_id);



cleanup:

    closesocket(fd);

    return ret;

}
