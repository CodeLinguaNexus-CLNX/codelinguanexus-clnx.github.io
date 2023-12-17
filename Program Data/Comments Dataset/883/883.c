/*
This function is responsible for receiving data from a TAP device and writing it to the file descriptor associated with the TAPState.

Here's the detail of the function:
- The function takes the TAPState pointer and a buffer containing data to be written as input parameters.
- Inside the function, it repeatedly attempts to write the data from the buffer to the file descriptor (s->fd) associated with the TAPState using the write system call.
- If the write operation is interrupted by a signal (EINTR) or if the file descriptor is non-blocking and the operation would block (EAGAIN), the function continues to attempt the write operation until it is successful.
- Once the data has been successfully written to the file descriptor, the loop breaks and the function returns.

Needs to notice:
- The function does not handle all possible error conditions arising from the write operation. It only retries the write operation in the case of EINTR or EAGAIN, without providing specific error handling or reporting.
- Proper error checking and handling should be implemented to deal with write failures or other potential errors, ensuring robustness and reliability in the data transfer process.
- Consideration should be given to the potential blocking nature of the write operation, especially if the file descriptor is set to non-blocking mode, to avoid potential performance issues or indefinite looping in the function.
- Verification of the buffer size and bounds checking should be considered to ensure that the correct amount of data is written and to prevent buffer overflows or other memory-related issues.
*/
static void tap_receive(void *opaque, const uint8_t *buf, size_t size)

{

    TAPState *s = opaque;

    int ret;

    for(;;) {

        ret = write(s->fd, buf, size);

        if (ret < 0 && (errno == EINTR || errno == EAGAIN)) {

        } else {

            break;

        }

    }

}
