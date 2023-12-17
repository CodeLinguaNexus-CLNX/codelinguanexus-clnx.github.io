/*
This function is responsible for determining the number of effective buses based on the configuration of a Xilinx SPI controller (XilinxSPIPS).

Here's the detail: Within the function, it evaluates the configuration of the SPI controller using specific bits from the 's->regs[R_LQSPI_STS]' register. If both the LQSPI_CFG_SEP_BUS and LQSPI_CFG_TWO_MEM bits are set, indicating separate buses and two memory devices are enabled, the function returns 's->num_busses'. Otherwise, it returns 1, indicating a single effective bus.

Need's to notice: It's important to understand the specific bit patterns and their meanings in the 's->regs[R_LQSPI_STS]' register, as well as the overall configuration of the Xilinx SPI controller, in order to correctly interpret the behavior of this function. Additionally, any changes to the SPI controller's configuration should be carefully considered to ensure that the function continues to provide the expected number of effective buses.
*/
static inline int num_effective_busses(XilinxSPIPS *s)

{

    return (s->regs[R_LQSPI_STS] & LQSPI_CFG_SEP_BUS &&

            s->regs[R_LQSPI_STS] & LQSPI_CFG_TWO_MEM) ? s->num_busses : 1;

}
