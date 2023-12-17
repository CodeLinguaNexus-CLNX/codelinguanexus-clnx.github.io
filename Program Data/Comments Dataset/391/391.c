/*
This function is responsible for attaching an I2S (Inter-IC Sound) codec to an OMAP McBSP (Multi-Channel Buffered Serial Port) instance.

Here's the detail:
- It first assigns the given I2S codec (slave) to the codec field of the OMAP McBSP structure (s).
- It then allocates an IRQ (interrupt request) for the RX (receive) operation of the I2S codec using qemu_allocate_irqs, with omap_mcbsp_i2s_swallow as the handler and the McBSP instance (s) as the opaque parameter. The allocated IRQ is stored in the rx_swallow field of the I2S codec.
- Similarly, an IRQ is allocated for the TX (transmit) start operation of the I2S codec, and the allocated IRQ is stored in the tx_start field of the I2S codec.

Needs to notice:
- This function assumes that the I2S codec and McBSP instance are valid and properly initialized. It does not perform error checking for NULL pointers or invalid configurations.
- The function directly assigns the allocated IRQs to the I2S codec structure, assuming a successful allocation. Proper error handling should be added to handle cases where IRQ allocation fails.
- The function relies on the qemu_allocate_irqs function to allocate IRQs for the I2S codec operations, and the behavior and availability of IRQs may vary based on the underlying system and configuration.
*/
void omap_mcbsp_i2s_attach(struct omap_mcbsp_s *s, I2SCodec *slave)

{

    s->codec = slave;

    slave->rx_swallow = qemu_allocate_irqs(omap_mcbsp_i2s_swallow, s, 1)[0];

    slave->tx_start = qemu_allocate_irqs(omap_mcbsp_i2s_start, s, 1)[0];

}
