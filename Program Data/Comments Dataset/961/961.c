/*
This function is responsible for processing a login request in the context of the VSCSI (Virtual Storage Client Interface) protocol.

Here's the detail:
The function operates on a given VSCSIState and a VSCSI request (req) as input. It begins by accessing the SRP (SCSI RDMA Protocol) login response structure within the request, then initializes and populates this structure with specific response data. This includes setting the response opcode, request limit delta, tag, maximum inbound transfer size, and buffer formats for direct and indirect data.

Additionally, the function calls vscsi_send_iu to send the constructed response to the VSCSI target.

Needs to notice:
1. The function currently contains a "TODO" comment indicating a potential handling for cases where the requested size is wrong or the buffer format is incorrect. It's important to address this as part of the function's completeness.
2. The specific protocol and data format details should be understood and handled correctly, as they directly impact the communication and behavior of the VSCSI protocol.
3. Proper error handling and response validation should be implemented to ensure the reliability and correctness of the login response processing.
*/
static void vscsi_process_login(VSCSIState *s, vscsi_req *req)

{

    union viosrp_iu *iu = &req->iu;

    struct srp_login_rsp *rsp = &iu->srp.login_rsp;

    uint64_t tag = iu->srp.rsp.tag;



    trace_spapr_vscsi__process_login();



    /* TODO handle case that requested size is wrong and

     * buffer format is wrong

     */

    memset(iu, 0, sizeof(struct srp_login_rsp));

    rsp->opcode = SRP_LOGIN_RSP;

    /* Don't advertise quite as many request as we support to

     * keep room for management stuff etc...

     */

    rsp->req_lim_delta = cpu_to_be32(VSCSI_REQ_LIMIT-2);

    rsp->tag = tag;

    rsp->max_it_iu_len = cpu_to_be32(sizeof(union srp_iu));

    rsp->max_ti_iu_len = cpu_to_be32(sizeof(union srp_iu));

    /* direct and indirect */

    rsp->buf_fmt = cpu_to_be16(SRP_BUF_FORMAT_DIRECT | SRP_BUF_FORMAT_INDIRECT);



    vscsi_send_iu(s, req, sizeof(*rsp), VIOSRP_SRP_FORMAT);

}
