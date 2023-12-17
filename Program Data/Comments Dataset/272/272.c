/*
This function is responsible for handling memory writes related to virtual interrupt delivery in a hardware virtualization context, and specifically interacts with the interrupt remapping subsystem.

Here's the detail:
1. Upon receiving a memory write request for a specific address (addr) with a given data value (value) and size, the vtd_mem_ir_write function initiates the handling of a virtual interrupt via the interrupt remapping mechanism in the virtualized environment.
2. It constructs an MSIMessage 'from' with the virtual interrupt address and data based on the provided addr and value, preparing the data for remapping and delivery to the target device.
3. The function calls vtd_interrupt_remap_msi to perform the interrupt remapping, which may involve translation and redirection of the virtual interrupt to the corresponding physical interrupt for the target device. It captures the remapped interrupt details in the MSIMessage 'to'.
4. Upon successful remapping, the function attempts to deliver the remapped MSI message to the target device's system ID (sid) using the dma_memory_write function, indicating the address and data values of the remapped interrupt.
5. In the event of any failures during the interrupt remapping, delivery, or memory write operation, the function reports errors using VTD_DPRINTF and handles the failure scenarios by returning MEMTX_ERROR and logging appropriate error messages.

Need's to notice:
1. This function plays a crucial role in the interrupt handling and remapping process within a virtualized system, necessitating a thorough understanding of the virtualization platform's interrupt delivery mechanisms and their interaction with the hardware.
2. The proper functioning of the interrupt remapping and delivery is vital for ensuring correct behavior and performance of virtualized devices, and any modifications to this function should be carefully validated and tested to maintain interrupt handling accuracy and reliability.
3. Error reporting and handling within this function are essential for diagnosing issues related to virtual interrupt delivery, and any error cases should be well-documented and handled appropriately to prevent potential system instability or data corruption.
4. Consideration should be given to the different virtualization scenarios and specific hardware configurations that may impact the interrupt remapping process, ensuring compatibility and robustness across diverse virtualized environments.
5. The proper integration of interrupt remapping with the virtualized environment's memory subsystem and device emulation is critical, and any changes to interrupt remapping behavior or interaction with memory writes should be evaluated within the broader context of virtualization and hardware emulation.
*/
static MemTxResult vtd_mem_ir_write(void *opaque, hwaddr addr,

                                    uint64_t value, unsigned size,

                                    MemTxAttrs attrs)

{

    int ret = 0;

    MSIMessage from = {0}, to = {0};



    from.address = (uint64_t) addr + VTD_INTERRUPT_ADDR_FIRST;

    from.data = (uint32_t) value;



    ret = vtd_interrupt_remap_msi(opaque, &from, &to);

    if (ret) {

        /* TODO: report error */

        VTD_DPRINTF(GENERAL, "int remap fail for addr 0x%"PRIx64

                    " data 0x%"PRIx32, from.address, from.data);

        /* Drop this interrupt */

        return MEMTX_ERROR;

    }



    VTD_DPRINTF(IR, "delivering MSI 0x%"PRIx64":0x%"PRIx32

                " for device sid 0x%04x",

                to.address, to.data, sid);



    if (dma_memory_write(&address_space_memory, to.address,

                         &to.data, size)) {

        VTD_DPRINTF(GENERAL, "error: fail to write 0x%"PRIx64

                    " value 0x%"PRIx32, to.address, to.data);

    }



    return MEMTX_OK;

}
