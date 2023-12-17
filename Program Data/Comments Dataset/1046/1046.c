/*
This function is responsible for preparing the KVM clock state for the VM (virtual machine) to be saved.

Here's the detail:
The function first retrieves the KVMClockState from the opaque pointer which is typically a pointer to the main KVM state structure. It then checks if the clock state is already valid, and if so, it returns early without taking any further action. If the clock state is not valid, it uses a KVM IOCTL (input/output control) request to retrieve the current clock data from the KVM state. If the IOCTL operation fails, it prints an error message to stderr and sets the clock value to 0. Regardless of the result of the IOCTL operation, it then updates the clock value in the KVMClockState with the retrieved or default value. Finally, it sets the clock state as valid if the VM is in a stopped state, to avoid re-reading it on the next VM save operation.

Need's to notice:
- This function operates in the context of the KVM (Kernel-based Virtual Machine) hypervisor and is likely part of the process of saving the state of a virtual machine.
- The function relies on the availability and proper functioning of the KVM IOCTL interface and can handle potential errors returned by the IOCTL operation.
- It's important to ensure that the KVMClockState structure and the underlying KVM state structure are properly initialized and in a consistent state before calling this function to avoid unexpected behavior.
- This function interacts closely with the virtual machine run state and makes decisions based on whether the VM is running or stopped. Therefore, the context of its usage within the broader VM lifecycle must be carefully considered for proper integration into a virtualization system.
*/
static void kvmclock_pre_save(void *opaque)

{

    KVMClockState *s = opaque;

    struct kvm_clock_data data;

    int ret;



    if (s->clock_valid) {

        return;

    }

    ret = kvm_vm_ioctl(kvm_state, KVM_GET_CLOCK, &data);

    if (ret < 0) {

        fprintf(stderr, "KVM_GET_CLOCK failed: %s\n", strerror(ret));

        data.clock = 0;

    }

    s->clock = data.clock;

    /*

     * If the VM is stopped, declare the clock state valid to avoid re-reading

     * it on next vmsave (which would return a different value). Will be reset

     * when the VM is continued.

     */

    s->clock_valid = !runstate_is_running();

}
