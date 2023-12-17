/*
This function is responsible for allocating resources for multithreading in the context of slice-based threading in an FFmpeg codec.

Here's the detail:
- The function takes an AVCodecContext *avctx and an integer count as input parameters, representing the codec context and the count of entries to allocate.
- It first checks if the active_thread_type in the codec context includes FF_THREAD_SLICE, indicating slice-based multithreading.
- If slice-based multithreading is active, the function initializes multithreading variables within the internal thread context of the codec.
- It allocates and clears an array of integers for entries using av_mallocz_array, and if the allocation fails, an out-of-memory error is returned.
- It allocates and initializes arrays of pthread_mutex_t and pthread_cond_t for progress reporting for each thread, with the array size based on the thread_count.
- It initializes each mutex and condition variable in the arrays using pthread_mutex_init and pthread_cond_init.

Needs to notice:
- The function assumes that slice-based multithreading is enabled and the internal thread context is already initialized within the codec context. Therefore, this function should be called appropriately within the context of multithreading setup.
- Memory allocation and deallocation for multithreading resources are managed within this function, requiring careful attention to resource release to avoid memory leaks.
- The use of low-level threading primitives such as pthread_mutex_t and pthread_cond_t necessitates understanding of multithreading concepts and potential risks associated with threading, such as deadlocks and race conditions.
- This function is specific to the FFmpeg framework and is intended for internal use within FFmpeg codec implementation, rather than for general-purpose multithreading.
*/
int ff_alloc_entries(AVCodecContext *avctx, int count)

{

    int i;



    if (avctx->active_thread_type & FF_THREAD_SLICE)  {

        SliceThreadContext *p = avctx->internal->thread_ctx;

        p->thread_count  = avctx->thread_count;

        p->entries       = av_mallocz_array(count, sizeof(int));



        if (!p->entries) {

            return AVERROR(ENOMEM);

        }



        p->entries_count  = count;

        p->progress_mutex = av_malloc_array(p->thread_count, sizeof(pthread_mutex_t));

        p->progress_cond  = av_malloc_array(p->thread_count, sizeof(pthread_cond_t));



        for (i = 0; i < p->thread_count; i++) {

            pthread_mutex_init(&p->progress_mutex[i], NULL);

            pthread_cond_init(&p->progress_cond[i], NULL);

        }

    }



    return 0;

}
