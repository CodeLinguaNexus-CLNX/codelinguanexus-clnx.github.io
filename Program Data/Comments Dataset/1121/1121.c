/*
This function is responsible for deleting a USB device with a specific address from the USB bus, and releasing its associated resources.

Here's the detail:
The function first finds the USB bus with the given bus number. If the bus is found, it iterates through the list of used ports on that bus to find the port associated with the specified device address. Once the port is found, it removes it from the list of used ports and decrements the count of used ports on the bus. Then it detaches the device from the port by calling usb_attach(), and invokes the handle_destroy() function of the device's information structure to release any resources associated with the device. Finally, it inserts the port into the list of free ports and increments the count of free ports on the bus.

Needs to notice:
Before calling this function, it's important to ensure that the bus number and device address provided are valid. Additionally, any additional cleanup or resource release specific to the application or device being deleted should be performed prior to calling this function. Special attention should be paid to ensure that no other parts of the code are using the device or its resources after it has been deleted.
*/
int usb_device_delete_addr(int busnr, int addr)

{

    USBBus *bus;

    USBPort *port;

    USBDevice *dev;



    bus = usb_bus_find(busnr);

    if (!bus)

        return -1;



    TAILQ_FOREACH(port, &bus->used, next) {

        if (port->dev->addr == addr)

            break;

    }

    if (!port)

        return -1;



    dev = port->dev;

    TAILQ_REMOVE(&bus->used, port, next);

    bus->nused--;



    usb_attach(port, NULL);

    dev->info->handle_destroy(dev);



    TAILQ_INSERT_TAIL(&bus->free, port, next);

    bus->nfree++;

    return 0;

}
