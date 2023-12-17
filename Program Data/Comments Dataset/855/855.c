/*
This function is responsible for preparing and adding a BIOS linker loader entry for memory allocation to the beginning of a GArray, which is used to manage firmware memory segments.

Here's the detail of the function:
- The function takes a GArray pointer for the linker, the file name, allocation alignment, and a boolean flag for determining the allocation zone as input parameters.
- It begins by creating a BiosLinkerLoaderEntry structure named entry and performing an assertion check to ensure that the allocation alignment is a power of 2.
- The entry structure is then initialized by setting the file name, allocation alignment, and allocation zone based on the input parameters. The file name is copied into the entry->alloc.file field, and the allocation alignment and zone are converted to little-endian format using the cpu_to_le32 function.
- After preparing the entry, it is added to the beginning of the linker array using g_array_prepend_vals, ensuring that allocation entries come first as indicated by the comment "/* Alloc entries must come first, so prepend them */".

Needs to notice :
-Understanding the BIOS linker loader protocol and the structure of BIOS linker loader entries is essential for correctly preparing and adding the allocation entries to the linker array.
- Proper validation and handling of input parameters, such as the file name, allocation alignment, and allocation zone, are important to ensure the correctness of the allocation entry.
- Careful consideration should be given to the organization of linker loader entries to ensure the proper ordering of entries within the linker array.
- Attention to resource management and potential memory allocation failures when working with GArray and memory allocation operations is necessary to prevent memory leaks and ensure the proper functioning of the linker loader.
*/
void bios_linker_loader_alloc(GArray *linker,

                              const char *file,

                              uint32_t alloc_align,

                              bool alloc_fseg)

{

    BiosLinkerLoaderEntry entry;



    assert(!(alloc_align & (alloc_align - 1)));



    memset(&entry, 0, sizeof entry);

    strncpy(entry.alloc.file, file, sizeof entry.alloc.file - 1);

    entry.command = cpu_to_le32(BIOS_LINKER_LOADER_COMMAND_ALLOCATE);

    entry.alloc.align = cpu_to_le32(alloc_align);

    entry.alloc.zone = cpu_to_le32(alloc_fseg ?

                                    BIOS_LINKER_LOADER_ALLOC_ZONE_FSEG :

                                    BIOS_LINKER_LOADER_ALLOC_ZONE_HIGH);



    /* Alloc entries must come first, so prepend them */

    g_array_prepend_vals(linker, &entry, sizeof entry);

}
