/*
This function is responsible for synchronizing the dirty bitmap for a specified memory range in the KVM environment to ensure that the virtual machine's memory dirty page information is up to date and consistent with the actual physical memory state.

Here's the detail:
- The function initializes necessary variables and structures, including the KVM state pointer, variables for tracking dirty bitmap size and allocated size, as well as instances of KVM-related data structures.
- It enters a loop to find the memory slot that overlaps with the current memory range and proceeds with further operations if a slot is found.
- Upon finding the memory slot, the function calculates and allocates or reallocates memory for the dirty bitmap based on the slot's memory size, initializing it appropriately.
- It then utilizes KVM-related ioctl functions to retrieve and update the dirty bitmap based on the memory start address and size.
- Finally, it frees the allocated dirty bitmap memory and returns the result of the operation.

Needs to notice:
- The function relies on other KVM-related functions and data structures, such as `kvm_lookup_overlapping_slot`, `kvm_vm_ioctl`, `KVMDirtyLog`, and `KVMSlot`.
- Proper error handling is implemented within the function, and memory allocations and deallocations are managed using `qemu_malloc` and `qemu_realloc` to ensure safe and reliable memory operations within the KVM environment.
*/
static int kvm_physical_sync_dirty_bitmap(target_phys_addr_t start_addr,

                                          target_phys_addr_t end_addr)

{

    KVMState *s = kvm_state;

    unsigned long size, allocated_size = 0;

    KVMDirtyLog d;

    KVMSlot *mem;

    int ret = 0;



    d.dirty_bitmap = NULL;

    while (start_addr < end_addr) {

        mem = kvm_lookup_overlapping_slot(s, start_addr, end_addr);

        if (mem == NULL) {

            break;

        }



        size = ALIGN(((mem->memory_size) >> TARGET_PAGE_BITS), HOST_LONG_BITS) / 8;

        if (!d.dirty_bitmap) {

            d.dirty_bitmap = qemu_malloc(size);

        } else if (size > allocated_size) {

            d.dirty_bitmap = qemu_realloc(d.dirty_bitmap, size);

        }

        allocated_size = size;

        memset(d.dirty_bitmap, 0, allocated_size);



        d.slot = mem->slot;



        if (kvm_vm_ioctl(s, KVM_GET_DIRTY_LOG, &d) == -1) {

            DPRINTF("ioctl failed %d\n", errno);

            ret = -1;

            break;

        }



        kvm_get_dirty_pages_log_range(mem->start_addr, d.dirty_bitmap,

                                      mem->start_addr, mem->memory_size);

        start_addr = mem->start_addr + mem->memory_size;

    }

    qemu_free(d.dirty_bitmap);



    return ret;

}
