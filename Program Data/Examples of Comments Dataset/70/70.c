/*
This function is responsible for setting the allocation state of a specific PAPR DRC (Dynamic Reconfiguration Connector) to "unusable".

Here's the detail:
- The function sets the allocation_state property of the given DRC to SPAPR_DR_ALLOCATION_STATE_UNUSABLE.
- If the DRC was awaiting release (marked by the awaiting_release flag), additional steps are taken.
  - The function retrieves the index of the DRC using the spapr_drc_index() function.
  - It traces the finalizing of setting the allocation state using the trace_spapr_drc_set_allocation_state_finalizing() function, passing the DRC index as an argument.
  - The spapr_drc_detach() function is called to detach the DRC, effectively finalizing the process of setting the allocation state.

Finally, the function returns RTAS_OUT_SUCCESS to indicate successful execution.

Need's to notice:
- This code assumes the existence of other functions and variables related to PAPR DRC management, which are not included in the given code snippet.
- The execution and the effect of this code may depend on the surrounding context and how the PAPR DRC is used in the larger codebase.
*/
static uint32_t drc_set_unusable(sPAPRDRConnector *drc)

{

    drc->allocation_state = SPAPR_DR_ALLOCATION_STATE_UNUSABLE;

    if (drc->awaiting_release) {

        uint32_t drc_index = spapr_drc_index(drc);

        trace_spapr_drc_set_allocation_state_finalizing(drc_index);

        spapr_drc_detach(drc);

    }



    return RTAS_OUT_SUCCESS;

}
