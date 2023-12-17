/*
This function is responsible for probing a device to determine its suitability for handling floppy disk operations.

Here's the detail:
- The function takes a string filename as input, representing the name of the device file to be probed.
- It initializes an integer prio (priority) to 0 and creates structures for storing floppy parameters and file information.
- The function checks if the filename starts with "/dev/fd" and does not start with "/dev/fdset/". If the condition is met, it sets the priority to 50, indicating a potential match for a floppy disk device.
- It attempts to open the specified file using qemu_open with read-only and non-blocking flags. If the open operation fails, the function exits.
- After successfully opening the file, it retrieves file information using fstat and verifies that the file represents a block device. If the file does not meet the criteria, the function exits.
- The function then tries to detect the device using a floppy-specific ioctl (input/output control) call with the FDGETPRM command, which retrieves the parameters for the floppy disk. If the ioctl operation succeeds, it sets the priority to 100, indicating a strong likelihood that the device is a floppy disk.

Needs to notice:
- The function relies on low-level system calls and ioctls, which may introduce platform-specific behavior and dependencies.
- Error handling is minimal, and potential errors from system calls such as ioctl, fstat, and open are not explicitly handled, which can lead to undefined behavior or unreported errors.
- The function assumes a certain file naming convention for floppy devices ("/dev/fd"), which may not be universally applicable across different systems.
- This function is specific to probing floppy devices and may need modification to support different types of block devices or additional probing logic for comprehensive device detection.
*/
static int floppy_probe_device(const char *filename)

{

    int fd, ret;

    int prio = 0;

    struct floppy_struct fdparam;

    struct stat st;



    if (strstart(filename, "/dev/fd", NULL) &&

        !strstart(filename, "/dev/fdset/", NULL)) {

        prio = 50;

    }



    fd = qemu_open(filename, O_RDONLY | O_NONBLOCK);

    if (fd < 0) {

        goto out;

    }

    ret = fstat(fd, &st);

    if (ret == -1 || !S_ISBLK(st.st_mode)) {

        goto outc;

    }



    /* Attempt to detect via a floppy specific ioctl */

    ret = ioctl(fd, FDGETPRM, &fdparam);

    if (ret >= 0)

        prio = 100;



outc:

    qemu_close(fd);

out:

    return prio;

}
