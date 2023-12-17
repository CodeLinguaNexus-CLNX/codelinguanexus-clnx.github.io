/*
This function is responsible for handling the opening of an iSCSI BlockDriver. It initializes the necessary data structures, parses the iSCSI URL, sets up the iSCSI context, and establishes a connection to the iSCSI LUN.

Here's the detail:
- The function first performs a check on the BDRV_SECTOR_SIZE to ensure it's a multiple of 512, reporting an error if it's not.
- It then proceeds to parse the iSCSI URL using the iscsi_parse_full_url function and handles any parsing failures.
- An initiator name is extracted from the iSCSI URL and used to create the iSCSI context. If the context creation fails, an error is reported, and the function returns an error code.
- Various attributes of the iSCSI context, such as the target name, initiator username/password, session type, and header digest, are set based on the parsed iSCSI URL.
- An asynchronous connection to the iSCSI LUN is initiated using iscsi_full_connect_async.
- The function then waits for the asynchronous connection to complete using qemu_aio_wait and checks for any connection failures.
- If the connection is successful, further settings are applied based on the type of the iSCSI LUN (such as medium changer or tape).
- Finally, the function cleans up allocated resources and returns either success or an error code.

Usage needs to notice:
- Developers using this function should have a good understanding of iSCSI concepts and the libiscsi library used for iSCSI operations.
- Careful handling of asynchronous operations and error conditions is crucial for robust iSCSI connection management.
- Configuration options and error handling related to iSCSI authentication mechanisms, such as CHAP, should be considered while integrating this function into a larger system.

If you have further questions, feel free to ask.
*/
static int iscsi_open(BlockDriverState *bs, const char *filename, int flags)

{

    IscsiLun *iscsilun = bs->opaque;

    struct iscsi_context *iscsi = NULL;

    struct iscsi_url *iscsi_url = NULL;

    struct IscsiTask task;

    char *initiator_name = NULL;

    int ret;



    if ((BDRV_SECTOR_SIZE % 512) != 0) {

        error_report("iSCSI: Invalid BDRV_SECTOR_SIZE. "

                     "BDRV_SECTOR_SIZE(%lld) is not a multiple "

                     "of 512", BDRV_SECTOR_SIZE);

        return -EINVAL;

    }



    iscsi_url = iscsi_parse_full_url(iscsi, filename);

    if (iscsi_url == NULL) {

        error_report("Failed to parse URL : %s %s", filename,

                     iscsi_get_error(iscsi));

        ret = -EINVAL;

        goto failed;

    }



    memset(iscsilun, 0, sizeof(IscsiLun));



    initiator_name = parse_initiator_name(iscsi_url->target);



    iscsi = iscsi_create_context(initiator_name);

    if (iscsi == NULL) {

        error_report("iSCSI: Failed to create iSCSI context.");

        ret = -ENOMEM;

        goto failed;

    }



    if (iscsi_set_targetname(iscsi, iscsi_url->target)) {

        error_report("iSCSI: Failed to set target name.");

        ret = -EINVAL;

        goto failed;

    }



    if (iscsi_url->user != NULL) {

        ret = iscsi_set_initiator_username_pwd(iscsi, iscsi_url->user,

                                              iscsi_url->passwd);

        if (ret != 0) {

            error_report("Failed to set initiator username and password");

            ret = -EINVAL;

            goto failed;

        }

    }



    /* check if we got CHAP username/password via the options */

    if (parse_chap(iscsi, iscsi_url->target) != 0) {

        error_report("iSCSI: Failed to set CHAP user/password");

        ret = -EINVAL;

        goto failed;

    }



    if (iscsi_set_session_type(iscsi, ISCSI_SESSION_NORMAL) != 0) {

        error_report("iSCSI: Failed to set session type to normal.");

        ret = -EINVAL;

        goto failed;

    }



    iscsi_set_header_digest(iscsi, ISCSI_HEADER_DIGEST_NONE_CRC32C);



    /* check if we got HEADER_DIGEST via the options */

    parse_header_digest(iscsi, iscsi_url->target);



    task.iscsilun = iscsilun;

    task.status = 0;

    task.complete = 0;

    task.bs = bs;



    iscsilun->iscsi = iscsi;

    iscsilun->lun   = iscsi_url->lun;



    if (iscsi_full_connect_async(iscsi, iscsi_url->portal, iscsi_url->lun,

                                 iscsi_connect_cb, &task)

        != 0) {

        error_report("iSCSI: Failed to start async connect.");

        ret = -EINVAL;

        goto failed;

    }



    while (!task.complete) {

        iscsi_set_events(iscsilun);

        qemu_aio_wait();

    }

    if (task.status != 0) {

        error_report("iSCSI: Failed to connect to LUN : %s",

                     iscsi_get_error(iscsi));

        ret = -EINVAL;

        goto failed;

    }



    if (iscsi_url != NULL) {

        iscsi_destroy_url(iscsi_url);

    }



    /* Medium changer or tape. We dont have any emulation for this so this must

     * be sg ioctl compatible. We force it to be sg, otherwise qemu will try

     * to read from the device to guess the image format.

     */

    if (iscsilun->type == TYPE_MEDIUM_CHANGER ||

        iscsilun->type == TYPE_TAPE) {

        bs->sg = 1;

    }



    return 0;



failed:

    if (initiator_name != NULL) {

        g_free(initiator_name);

    }

    if (iscsi_url != NULL) {

        iscsi_destroy_url(iscsi_url);

    }

    if (iscsi != NULL) {

        iscsi_destroy_context(iscsi);

    }

    memset(iscsilun, 0, sizeof(IscsiLun));

    return ret;

}
