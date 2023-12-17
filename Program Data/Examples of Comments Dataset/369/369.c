/*
This function is responsible for initiating a streaming block job within the QEMU virtualization software. It takes input parameters such as job ID, block driver states, backing file, speed, error handling, callback function, and opaque data to start the streaming process.

Here's the detail:
- Upon invocation, the function creates a StreamBlockJob object using the block_job_create function, providing the job ID, block driver state, speed, callback function, opaque data, and error pointer.
- It then sets the base block driver state, backing file string, error handling function, and creates a coroutine for the streaming process using qemu_coroutine_create.
- The function enters the coroutine and initiates the streaming process by calling the stream_run function with the provided StreamBlockJob object.

Needs to notice:
- The function expects valid input parameters such as job ID, block driver states, and callback function to initiate the streaming job.
- It creates a coroutine to handle the asynchronous nature of the streaming process.
- Proper error handling should be ensured when using this function, as it can potentially encounter errors during the streaming process.
*/
void stream_start(const char *job_id, BlockDriverState *bs,

                  BlockDriverState *base, const char *backing_file_str,

                  int64_t speed, BlockdevOnError on_error,

                  BlockCompletionFunc *cb, void *opaque, Error **errp)

{

    StreamBlockJob *s;



    s = block_job_create(job_id, &stream_job_driver, bs, speed,

                         cb, opaque, errp);

    if (!s) {

        return;

    }



    s->base = base;

    s->backing_file_str = g_strdup(backing_file_str);



    s->on_error = on_error;

    s->common.co = qemu_coroutine_create(stream_run);

    trace_stream_start(bs, base, s, s->common.co, opaque);

    qemu_coroutine_enter(s->common.co, s);

}
