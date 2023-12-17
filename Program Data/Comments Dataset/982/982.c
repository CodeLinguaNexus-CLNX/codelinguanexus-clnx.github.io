/*
This function is responsible for checking whether a given block driver state represents a removable device.

Here's the detail:
The function takes a BlockDriverState pointer as input and returns the value of the 'removable' field within the structure. The 'removable' field is a boolean flag that indicates whether the block driver state represents a removable device (e.g., a USB drive, an external hard disk, etc.). The function simply returns the value of this flag, providing a quick way to determine the removability of the device.

Need's to notice:
- The function assumes that the BlockDriverState pointer 'bs' is valid and properly initialized.
- It's important to consider the context in which this function is used to ensure that the result is interpreted correctly. For example, it's important to handle cases where the device may have been marked as removable, but is not currently physically removable.
- The return value of this function can be used to make decisions based on the removability of the device, but it should be used judiciously, taking into account the potential for false positives or false negatives based on the system configuration and device status.
*/
int bdrv_is_removable(BlockDriverState *bs)

{

    return bs->removable;

}
