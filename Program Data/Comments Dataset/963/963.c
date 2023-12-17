/*
This function is responsible for setting the count value for a CPU timer and updating related properties based on the provided count.

Here's the detail:
The function takes a CPUTimer pointer and a 64-bit count value as input. It first extracts the real count and the disabled bit from the provided count. Then, it calculates the virtual machine clock offset based on the current virtual clock and the converted real count using the timer's frequency. After that, it updates the timer's properties, including the disabled flag and the clock offset. Finally, it emits a debug message to indicate the count setting operation and related details.

Needs to notice:
1. The function directly manipulates the disabled flag based on the presence of the disabled bit in the count value. It's important to ensure that this behavior aligns with the intended logic for controlling the timer's enabled/disabled state.
2. Understanding the relationship between the count value and the virtual machine clock offset is crucial for correct operation. Any changes to this calculation should be carefully reviewed to maintain accurate timekeeping.
3. Proper boundary and error condition handling should be considered, especially when updating the timer properties, to maintain the consistency and integrity of the timer state.
*/
void cpu_tick_set_count(CPUTimer *timer, uint64_t count)

{

    uint64_t real_count = count & ~timer->disabled_mask;

    uint64_t disabled_bit = count & timer->disabled_mask;



    int64_t vm_clock_offset = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) -

                    cpu_to_timer_ticks(real_count, timer->frequency);



    TIMER_DPRINTF("%s set_count count=0x%016lx (%s) p=%p\n",

                  timer->name, real_count,

                  timer->disabled?"disabled":"enabled", timer);



    timer->disabled = disabled_bit ? 1 : 0;

    timer->clock_offset = vm_clock_offset;

}
