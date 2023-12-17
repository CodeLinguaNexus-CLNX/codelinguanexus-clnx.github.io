/*
This function is responsible for reading a 32-bit value from the Generic Interrupt Controller (GIC) distributor at a specified offset.

Here's the detail:
1. The function starts by declaring a local variable, val, of type uint32_t to hold the resulting value.
2. It calls the gic_dist_readb function, passing the opaque pointer and the provided offset, to read a single byte value from the GIC distributor.
3. The function then performs a bitwise OR operation with the previously-read value (val) and the result of reading another byte from the GIC distributor, offset + 1, shifted left by 8 bits.
4. Finally, the resulting value, val, is returned.

Need's to notice:
- The opaque parameter represents a pointer to the GIC distributor's internal data structure or context, which should be properly initialized and associated with the GIC before calling this function.
- The offset parameter specifies the memory location within the GIC distributor's address space from which to read the 32-bit value. This offset should be valid and within the appropriate memory range.
- The gic_dist_readb function is assumed to read a single byte from the specified GIC distributor offset.
- It's important to ensure the proper initialization and configuration of the GIC before using this function, as incorrect setup or invalid offsets may result in undefined behavior or incorrect values being read.
- This function assumes little-endian byte ordering, where the least significant byte is read first and combined with the upper byte to form the 32-bit value. If a different byte ordering is expected, appropriate modifications should be made to this code.
- Any potential errors or exceptional conditions during the reading process should be handled appropriately, maintaining the correctness and reliability of the GIC's functionality.
*/
static uint32_t gic_dist_readw(void *opaque, target_phys_addr_t offset)

{

    uint32_t val;

    val = gic_dist_readb(opaque, offset);

    val |= gic_dist_readb(opaque, offset + 1) << 8;

    return val;

}
