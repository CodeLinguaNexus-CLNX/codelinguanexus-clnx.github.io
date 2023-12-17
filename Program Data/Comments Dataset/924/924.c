/*
This function is responsible for registering core RTAS (Run-Time Abstraction Services) functions within the SPAPR (Shared Power Architecture Platform Reference) platform.

Here's the detail:
The function utilizes the spapr_rtas_register function to register specific RTAS functions, namely "display-character", "get-time-of-day", and "power-off", along with their corresponding handler functions: rtas_display_character, rtas_get_time_of_day, and rtas_power_off.

Needs to notice:
1. The SPAPR platform likely utilizes these RTAS functions for interfacing with the underlying hardware and providing essential runtime services.
2. It's important to ensure that the handler functions (rtas_display_character, rtas_get_time_of_day, and rtas_power_off) are correctly implemented to handle the respective RTAS calls.
3. Any additional core RTAS functions required by the SPAPR platform should be registered using similar mechanisms for complete functionality.
*/
static void register_core_rtas(void)

{

    spapr_rtas_register("display-character", rtas_display_character);

    spapr_rtas_register("get-time-of-day", rtas_get_time_of_day);

    spapr_rtas_register("power-off", rtas_power_off);




}