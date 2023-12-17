/*
This function is responsible for starting a mirror job for block device replication, handling parameters such as source and target block driver states, error handling, buffer size, and other job-specific settings.

Here's the detail:
1. The function 'mirror_start_job' takes various input parameters to initialize and start a mirror block job for replicating data from the source block driver to the target block driver.

2. It validates and sets default values for parameters such as granularity, buffer size, error handling, and other job-specific settings, ensuring the proper configuration of the mirror job.

3. It handles the replacement of block driver states, checks for the availability of block backends, and creates a dirty bitmap for tracking the modified data during the replication process.

4. The function configures the target block driver state, enables write cache, sets error handling, and initializes the mirror job's coroutine for execution.

Need's to notice:
It's crucial to provide valid and properly initialized input parameters to ensure the correct setup and execution of the mirror job. Additionally, error handling, buffer size, and granularity should be carefully configured to align with the desired replication behavior and performance considerations. It's important to ensure that the source and target block driver states are correctly configured and compatible with the mirror job requirements. Special attention should be given to potential error conditions and edge cases to ensure the robustness and reliability of the mirror job execution.
*/
static void mirror_start_job(BlockDriverState *bs, BlockDriverState *target,

                             const char *replaces,

                             int64_t speed, uint32_t granularity,

                             int64_t buf_size,

                             BlockdevOnError on_source_error,

                             BlockdevOnError on_target_error,

                             bool unmap,

                             BlockCompletionFunc *cb,

                             void *opaque, Error **errp,

                             const BlockJobDriver *driver,

                             bool is_none_mode, BlockDriverState *base)

{

    MirrorBlockJob *s;




    if (granularity == 0) {

        granularity = bdrv_get_default_bitmap_granularity(target);

    }



    assert ((granularity & (granularity - 1)) == 0);



    if ((on_source_error == BLOCKDEV_ON_ERROR_STOP ||

         on_source_error == BLOCKDEV_ON_ERROR_ENOSPC) &&

        (!bs->blk || !blk_iostatus_is_enabled(bs->blk))) {

        error_setg(errp, QERR_INVALID_PARAMETER, "on-source-error");

        return;

    }



    if (buf_size < 0) {

        error_setg(errp, "Invalid parameter 'buf-size'");

        return;

    }



    if (buf_size == 0) {

        buf_size = DEFAULT_MIRROR_BUF_SIZE;

    }



    /* We can't support this case as long as the block layer can't handle

     * multiple BlockBackends per BlockDriverState. */

    if (replaces) {

        replaced_bs = bdrv_lookup_bs(replaces, replaces, errp);

        if (replaced_bs == NULL) {

            return;

        }

    } else {

        replaced_bs = bs;

    }

    if (replaced_bs->blk && target->blk) {

        error_setg(errp, "Can't create node with two BlockBackends");

        return;

    }



    s = block_job_create(driver, bs, speed, cb, opaque, errp);

    if (!s) {

        return;

    }



    s->replaces = g_strdup(replaces);

    s->on_source_error = on_source_error;

    s->on_target_error = on_target_error;

    s->target = target;

    s->is_none_mode = is_none_mode;

    s->base = base;

    s->granularity = granularity;

    s->buf_size = ROUND_UP(buf_size, granularity);

    s->unmap = unmap;



    s->dirty_bitmap = bdrv_create_dirty_bitmap(bs, granularity, NULL, errp);

    if (!s->dirty_bitmap) {

        g_free(s->replaces);

        block_job_unref(&s->common);

        return;

    }



    bdrv_op_block_all(s->target, s->common.blocker);



    bdrv_set_enable_write_cache(s->target, true);

    if (s->target->blk) {

        blk_set_on_error(s->target->blk, on_target_error, on_target_error);

        blk_iostatus_enable(s->target->blk);

    }

    s->common.co = qemu_coroutine_create(mirror_run);

    trace_mirror_start(bs, s, s->common.co, opaque);

    qemu_coroutine_enter(s->common.co, s);

}