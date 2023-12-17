/*
This function is responsible for registering PL061 devices for use within the system bus.

Here's the detail:
- The function utilizes the sysbus_register_dev function to register two different PL061 devices: "pl061" and "pl061_luminary".
- Each registration specifies the device type, the size of the device state, and an initialization function specific to the device type.

Usage needs to notice:
- When developing or modifying this code, ensure that the initialization functions (pl061_init_arm and pl061_init_luminary) correspond correctly to the respective device types being registered.
- Understand the implications of registering different PL061 devices within the system bus and how they interact with other components within the system.
- Ensure that the registration process aligns with the overall system architecture and requirements for PL061 devices.
*/
static void pl061_register_devices(void)

{

    sysbus_register_dev("pl061", sizeof(pl061_state),

                        pl061_init_arm);

    sysbus_register_dev("pl061_luminary", sizeof(pl061_state),

                        pl061_init_luminary);

}
