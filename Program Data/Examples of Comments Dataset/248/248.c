/*
This function is responsible for handling USBRedir bulk packets received from a USB device within the USBRedirDevice module. It processes the incoming bulk packet, performs validation and manipulation of the packet data, and manages the associated asynchronous USB Request Blocks (URBs) for bulk data transfer.

Here's the detail:
1. The usbredir_bulk_packet function takes several parameters including a pointer to USBRedirDevice (priv), the packet id, a struct representing the bulk packet header, pointers to data and its length.
2. It begins by extracting the endpoint and length of the bulk packet, then locates the corresponding AsyncURB associated with the packet id.
3. If no AsyncURB is found, the function frees the data buffer and returns.
4. Otherwise, it checks for consistency between the received bulk packet and the AsyncURB, handling mismatch cases and logging errors if necessary.
5. If the AsyncURB's packet is valid, the function handles the packet status, logs the incoming bulk data, copies the received data to the packet buffer (with error handling for buffer size), updates the packet length, and marks the USB packet as complete.
6. Finally, it frees the AsyncURB and the data buffer.

Need's to notice:
1. The function involves significant protocol-specific processing and error handling related to USBRedir bulk packet reception and URB management within the USBRedirDevice.
2. It ensures that received bulk packet data is correctly associated with the relevant URB and handles potential mismatches or errors accordingly.
3. The function performs memory management for the data buffer and URB resources, including potential error paths and cleanup after processing.
4. Any modifications to this function should carefully consider the potential impact on USBRedir bulk packet handling and related URB management, ensuring the correctness of data processing, error handling, and resource deallocation.
5. It's important to maintain protocol compliance and handle potential error conditions from the USB device or the USBRedir protocol robustly.
*/
static void usbredir_bulk_packet(void* priv, uint32_t id,

    struct usb_redir_bulk_packet_header *bulk_packet,

    uint8_t *data, int data_len)

{

    USBRedirDevice *dev = priv;

    uint8_t ep = bulk_packet->endpoint;

    int len = bulk_packet->length;

    AsyncURB *aurb;



    DPRINTF("bulk-in status %d ep %02X len %d id %u\n", bulk_packet->status,

            ep, len, id);



    aurb = async_find(dev, id);

    if (!aurb) {

        free(data);

        return;

    }



    if (aurb->bulk_packet.endpoint != bulk_packet->endpoint ||

            aurb->bulk_packet.stream_id != bulk_packet->stream_id) {

        ERROR("return bulk packet mismatch, please report this!\n");

        len = USB_RET_NAK;

    }



    if (aurb->packet) {

        len = usbredir_handle_status(dev, bulk_packet->status, len);

        if (len > 0) {

            usbredir_log_data(dev, "bulk data in:", data, data_len);

            if (data_len <= aurb->packet->len) {

                memcpy(aurb->packet->data, data, data_len);

            } else {

                ERROR("bulk buffer too small (%d > %d)\n", data_len,

                      aurb->packet->len);

                len = USB_RET_STALL;

            }

        }

        aurb->packet->len = len;

        usb_packet_complete(&dev->dev, aurb->packet);

    }

    async_free(dev, aurb);

    free(data);

}
