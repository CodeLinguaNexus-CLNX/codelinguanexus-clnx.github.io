/*
This function is responsible for initializing the floating-point unit (FPU) for a MIPS CPU state.

Here's the detail:
- It iterates through the available FPUs and initializes the FCR0 register for each FPU based on the provided MIPS definition.
- It then copies the configuration of the first FPU to the active FPU for the current CPU state.
- If the CPU is operating in user mode only, it checks the CP0_Config1 register for the presence of the FPU, and sets the MIPS_HFLAG_FPU flag in the CPU environment's hflags field accordingly.
- For MIPS64 architecture, it also checks if the active FPU supports 64-bit floating-point operations and sets the MIPS_HFLAG_F64 flag when applicable.

Usage needs to notice:
- Ensure that the mips_def_t structure is properly initialized before calling this function.
- Understand the impact of setting the FPU configuration and the implications of enabling the FPU and 64-bit floating-point support based on the CPU mode and architecture.
*/
static void fpu_init (CPUMIPSState *env, const mips_def_t *def)

{

    int i;



    for (i = 0; i < MIPS_FPU_MAX; i++)

        env->fpus[i].fcr0 = def->CP1_fcr0;



    memcpy(&env->active_fpu, &env->fpus[0], sizeof(env->active_fpu));

    if (env->user_mode_only) {

        if (env->CP0_Config1 & (1 << CP0C1_FP))

            env->hflags |= MIPS_HFLAG_FPU;

#ifdef TARGET_MIPS64

        if (env->active_fpu.fcr0 & (1 << FCR0_F64))

            env->hflags |= MIPS_HFLAG_F64;

#endif

    }

}
