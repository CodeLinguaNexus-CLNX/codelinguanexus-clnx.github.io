/*
This function is responsible for invalidating the Translation Lookaside Buffers (TLB) for both the User TLB (UTLB) and the Instruction TLB (ITLB) in the CPU SH4 state.

Here's the detail: The function iterates through the entries of both the UTLB and ITLB, setting the "valid" flag of each entry to 0, effectively invalidating the TLB entries. It then calls the tlb_flush function with a parameter value of 1.

Need's to notice that the generated token in the comments should not exceed 256 characters in length.

*/ void cpu_sh4_invalidate_tlb(CPUSH4State* s)

{

    int i;



    /* UTLB */

    for (i = 0; i < UTLB_SIZE; i++) {

        tlb_t * entry = &s->utlb[i];

        entry->v = 0;

    }

    /* ITLB */

    for (i = 0; i < UTLB_SIZE; i++) {

        tlb_t * entry = &s->utlb[i];

        entry->v = 0;

    }



    tlb_flush(s, 1);

}
