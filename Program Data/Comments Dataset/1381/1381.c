/*
This function is responsible for entering the next coroutine in the given CoQueue.

Here's the detail:
- It takes a pointer to a CoQueue structure as the parameter.
- It retrieves the first coroutine from the queue using the QSIMPLEQ_FIRST macro.
- If there are no coroutines in the queue (i.e., next is NULL), it returns false.
- If there is a coroutine in the queue, it removes the first coroutine from the queue using QSIMPLEQ_REMOVE_HEAD and the co_queue_next field.
- It then enters the removed coroutine using the qemu_coroutine_enter function with NULL as the data.
- Finally, it returns true to indicate that a coroutine was entered.

Needs to notice:
- This function assumes that proper synchronization mechanisms are in place when accessing the CoQueue to avoid concurrent access issues.
- It is important to ensure that the CoQueue is initialized and managed correctly to avoid potential memory errors or undefined behavior.
- The behavior of the qemu_coroutine_enter function and its impact on the rest of the system should be considered when using this function in a larger context.
*/
bool qemu_co_enter_next(CoQueue *queue)

{

    Coroutine *next;



    next = QSIMPLEQ_FIRST(&queue->entries);

    if (!next) {

        return false;

    }



    QSIMPLEQ_REMOVE_HEAD(&queue->entries, co_queue_next);

    qemu_coroutine_enter(next, NULL);

    return true;

}
