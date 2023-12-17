/*
This function is responsible for initializing the MPTSAS SCSI controller device.

Here's the detail:
- The function, mptsas_scsi_init, takes a PCIDevice pointer (dev) and an Error pointer (errp) as parameters.
- Within the function, it initializes the PCI configuration registers by setting PCI_LATENCY_TIMER to 0 and PCI_INTERRUPT_PIN to 0x01.
- It then initializes three memory regions for the MMIO, I/O port, and diagnostics functionality of the MPTSAS device using memory_region_init_io.
- If MSI (Message Signaled Interrupts) is enabled and the initialization is successful, it sets the msi_in_use flag to true to indicate MSI usage.
- The function registers the initialized memory regions as PCI BARs (Base Address Registers) using pci_register_bar for I/O port, MMIO, and diagnostics regions.
- If the SAS address of the MPTSAS device is not already initialized, it constructs a default SAS address based on the device's PCI bus number, slot, and function.
- Additionally, it initializes the maximum number of devices supported by the MPTSAS device, creates a new bottom half (BH) for asynchronous event processing, and initializes the SCSI bus associated with the device.
- If the device is not hotplugged, it handles SCSI legacy command line parameters.

Need's to notice:
- This function is specific to initializing the MPTSAS SCSI controller device within the QEMU virtualization environment.
- It handles various aspects of device initialization, including PCI configuration, memory region initialization, MSI handling, BAR registration, SAS address assignment, asynchronous event processing, and SCSI bus setup.
- Understanding the behavior of this function is important for device emulation and management within virtualized environments, especially when working with SCSI controllers and storage devices.
*/
static void mptsas_scsi_init(PCIDevice *dev, Error **errp)

{

    DeviceState *d = DEVICE(dev);

    MPTSASState *s = MPT_SAS(dev);



    dev->config[PCI_LATENCY_TIMER] = 0;

    dev->config[PCI_INTERRUPT_PIN] = 0x01;



    memory_region_init_io(&s->mmio_io, OBJECT(s), &mptsas_mmio_ops, s,

                          "mptsas-mmio", 0x4000);

    memory_region_init_io(&s->port_io, OBJECT(s), &mptsas_port_ops, s,

                          "mptsas-io", 256);

    memory_region_init_io(&s->diag_io, OBJECT(s), &mptsas_diag_ops, s,

                          "mptsas-diag", 0x10000);



    if (s->msi != ON_OFF_AUTO_OFF &&

        msi_init(dev, 0, 1, true, false) >= 0) {

        /* TODO check for errors */

        s->msi_in_use = true;

    }



    pci_register_bar(dev, 0, PCI_BASE_ADDRESS_SPACE_IO, &s->port_io);

    pci_register_bar(dev, 1, PCI_BASE_ADDRESS_SPACE_MEMORY |

                                 PCI_BASE_ADDRESS_MEM_TYPE_32, &s->mmio_io);

    pci_register_bar(dev, 2, PCI_BASE_ADDRESS_SPACE_MEMORY |

                                 PCI_BASE_ADDRESS_MEM_TYPE_32, &s->diag_io);



    if (!s->sas_addr) {

        s->sas_addr = ((NAA_LOCALLY_ASSIGNED_ID << 24) |

                       IEEE_COMPANY_LOCALLY_ASSIGNED) << 36;

        s->sas_addr |= (pci_bus_num(dev->bus) << 16);

        s->sas_addr |= (PCI_SLOT(dev->devfn) << 8);

        s->sas_addr |= PCI_FUNC(dev->devfn);

    }

    s->max_devices = MPTSAS_NUM_PORTS;



    s->request_bh = qemu_bh_new(mptsas_fetch_requests, s);



    QTAILQ_INIT(&s->pending);



    scsi_bus_new(&s->bus, sizeof(s->bus), &dev->qdev, &mptsas_scsi_info, NULL);

    if (!d->hotplugged) {

        scsi_bus_legacy_handle_cmdline(&s->bus, errp);

    }

}
