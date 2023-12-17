/*
This function is responsible for triggering an invalidation of the I/O Translation Lookaside Buffer (IOTLB) for a specified range of memory addresses associated with a vhost device in a virtualized environment.

Here's the detail:
1. The function creates an IOTLB invalidation message struct (vhost_iotlb_msg) and initializes its fields with the provided I/O virtual address (iova), length (len), and type of invalidation (VHOST_IOTLB_INVALIDATE).
2. It uses the vhost_send_device_iotlb_msg callback from the vhost_ops function table to send the constructed IOTLB invalidation message to the vhost device. This operation informs the device that the specified memory range is no longer valid or should be updated.

Need's to notice:
- The provided vhost_dev structure (dev) should be properly initialized and associated with a valid vhost device before invoking this function, ensuring that the device is in a consistent and operational state.
- It's essential to validate the input parameters (iova and len) to ensure that the IOTLB invalidation message accurately reflects the memory range that requires invalidation and that the length is within the valid address space.
- The behavior of the vhost_send_device_iotlb_msg function and its interaction with the underlying vhost device should be well-understood, as any potential side effects or asynchronous behaviors could impact system stability and correctness.
- Care should be taken to handle any potential errors or exceptional conditions that may arise during the process of sending the IOTLB invalidation message, maintaining the reliability and integrity of the virtualized environment.
*/
int vhost_backend_invalidate_device_iotlb(struct vhost_dev *dev,

                                                 uint64_t iova, uint64_t len)

{

    struct vhost_iotlb_msg imsg;



    imsg.iova = iova;

    imsg.size = len;

    imsg.type = VHOST_IOTLB_INVALIDATE;



    return dev->vhost_ops->vhost_send_device_iotlb_msg(dev, &imsg);

}
