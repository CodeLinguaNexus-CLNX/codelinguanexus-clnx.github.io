/*
This function is responsible for emulating the process of adding a Translation Control Entry (TCE) to a specific PAPR TCE table for IOMMU translation.

Here's the detail:

The function takes a PAPRTCETable pointer, an I/O Bus Address (IOBA), and a TCE (Translation Control Entry) as input parameters.
It calculates the index of the TCE table based on the IOBA and performs a boundary check to ensure the index is within the valid range.
If the index is out of bounds, an error message is printed using hcall_dprintf, and the function returns H_PARAMETER to indicate a parameter-related error.
If the index is within bounds, the TCE value is stored in the TCE table at the calculated index.
An IOMMUTLBEntry structure is initialized with relevant information including the target address space, translated address, address mask, and permissions.
The function then notifies the IOMMU about the memory region using memory_region_notify_iommu, passing in the IOMMU instance and the initialized IOMMUTLBEntry.
Finally, the function returns H_SUCCESS to indicate a successful TCE emulation operation.

Points to notice:

The function encapsulates the logic for handling TCE table updates and IOMMU notifications within a PAPR TCE table emulation context.
It relies on specific data structures and functions such as PAPRTCETable, IOMMUTLBEntry, memory_region_notify_iommu, and spapr_tce_iommu_access_flags to emulate TCE operations effectively.
Error handling is performed when the input index is found to be out of bounds, ensuring the integrity of the TCE table update process.
*/
static target_ulong put_tce_emu(sPAPRTCETable *tcet, target_ulong ioba,

                                target_ulong tce)

{

    IOMMUTLBEntry entry;

    hwaddr page_mask = IOMMU_PAGE_MASK(tcet->page_shift);

    unsigned long index = (ioba - tcet->bus_offset) >> tcet->page_shift;



    if (index >= tcet->nb_table) {

        hcall_dprintf("spapr_vio_put_tce on out-of-bounds IOBA 0x"

                      TARGET_FMT_lx "\n", ioba);

        return H_PARAMETER;

    }



    tcet->table[index] = tce;



    entry.target_as = &address_space_memory,

    entry.iova = ioba & page_mask;

    entry.translated_addr = tce & page_mask;

    entry.addr_mask = ~page_mask;

    entry.perm = spapr_tce_iommu_access_flags(tce);

    memory_region_notify_iommu(&tcet->iommu, entry);



    return H_SUCCESS;

}
