/*
This function is responsible for allocating a temporary frame for a TCG context in a QEMU-based emulator.

Here's the detail:
- The function takes a TCGContext pointer (s) and an integer (temp) as parameters.
- It retrieves the TCGTemp structure for the given temporary index from the TCGContext's temps array.
- It then aligns the current frame offset to the size of a tcg_target_long (which is the target architecture's long integer size) and updates it. This ensures proper alignment for the next allocated frame.
- Next, it checks if the current frame offset plus the size of a tcg_target_long exceeds the frame end, and if so, it aborts the code generation (tcg_abort()).
- If the allocation is within bounds, it updates the memory offset and memory register for the TCGTemp, marks it as allocated, and increments the current frame offset by the size of a tcg_target_long.

Need's to notice:
- This function is responsible for low-level memory allocation in the context of QEMU's Tiny Code Generator (TCG) for generating target machine code.
- It operates on the TCGContext's internal state and memory allocation, ensuring proper alignment and bounds checking for temporary frame allocation.
- The function assumes that the necessary structures within the TCGContext are properly initialized and managed before calling this function.
- It's important to ensure that the frame allocation logic aligns with the target architecture's memory alignment requirements to avoid alignment-related issues in generated machine code.
*/
static void temp_allocate_frame(TCGContext *s, int temp)

{

    TCGTemp *ts;

    ts = &s->temps[temp];

    s->current_frame_offset = (s->current_frame_offset + sizeof(tcg_target_long) - 1) & ~(sizeof(tcg_target_long) - 1);

    if (s->current_frame_offset + sizeof(tcg_target_long) > s->frame_end)

        tcg_abort();

    ts->mem_offset = s->current_frame_offset;

    ts->mem_reg = s->frame_reg;

    ts->mem_allocated = 1;

    s->current_frame_offset += sizeof(tcg_target_long);

}
