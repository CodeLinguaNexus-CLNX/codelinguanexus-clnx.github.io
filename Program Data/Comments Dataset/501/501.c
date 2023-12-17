/*
This function is responsible for submitting a task to the thread pool for asynchronous execution.

Here's the detail:
- It is a convenience wrapper for the thread_pool_submit_aio function, with the additional parameters set to NULL.
- The function takes a function pointer and an argument, and submits them to the thread pool for asynchronous execution using the thread_pool_submit_aio function.

Usage needs to notice:
- When using this function, ensure that the provided func and arg parameters are appropriate for the task to be executed asynchronously.
- Understand the behavior of the underlying thread pool and how the submitted tasks will be executed asynchronously.
- Handle any potential errors or exceptions that may occur during the execution of the submitted task.
*/
void thread_pool_submit(ThreadPoolFunc *func, void *arg)

{

    thread_pool_submit_aio(func, arg, NULL, NULL);

}
