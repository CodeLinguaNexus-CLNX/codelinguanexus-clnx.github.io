/*
This function is responsible for calculating the checksum for an incoming packet in the context of the VMXNET3 virtual network device. It checks if the packet requires checksum calculation based on the Virtio header flags and the packet's protocols, and then proceeds to calculate and update the checksum if needed.

Here's the detail:
- The function first checks if the incoming packet has a Virtio header. If not, it returns early as checksum calculation is not applicable.
- It then checks the Virtio header flags to determine if checksum calculation is needed. If not, it returns as well.
- Next, it examines the packet's protocols (IP and either TCP or UDP) to ensure that checksum calculation is applicable for the packet. If not, it returns early again.
- After these checks, the function proceeds to validate the packet length to ensure it's sufficient for checksum calculation.
- If the length is valid, it calculates the checksum and updates the packet with the obtained checksum value.
- Finally, it updates the Virtio header flags accordingly to indicate that the checksum is calculated and valid.

Usage needs to notice:
- Developers using this function should have a good understanding of network packet structures, protocols, and checksum calculation.
- The function assumes familiarity with the VMXNET3 virtual network device and its specific packet handling requirements.
- Careful consideration should be given to packet length validation and proper handling of different protocol types.
- Developers need to ensure synchronization and proper memory access when updating packet data.

If you have further questions, feel free to ask.
*/
static void vmxnet3_rx_need_csum_calculate(struct VmxnetRxPkt *pkt,

                                           const void *pkt_data,

                                           size_t pkt_len)

{

    struct virtio_net_hdr *vhdr;

    bool isip4, isip6, istcp, isudp;

    uint8_t *data;

    int len;



    if (!vmxnet_rx_pkt_has_virt_hdr(pkt)) {

        return;

    }



    vhdr = vmxnet_rx_pkt_get_vhdr(pkt);

    if (!VMXNET_FLAG_IS_SET(vhdr->flags, VIRTIO_NET_HDR_F_NEEDS_CSUM)) {

        return;

    }



    vmxnet_rx_pkt_get_protocols(pkt, &isip4, &isip6, &isudp, &istcp);

    if (!(isip4 || isip6) || !(istcp || isudp)) {

        return;

    }



    vmxnet3_dump_virt_hdr(vhdr);



    /* Validate packet len: csum_start + scum_offset + length of csum field */

    if (pkt_len < (vhdr->csum_start + vhdr->csum_offset + 2)) {

        VMW_PKPRN("packet len:%d < csum_start(%d) + csum_offset(%d) + 2, "

                  "cannot calculate checksum",

                  len, vhdr->csum_start, vhdr->csum_offset);

        return;

    }



    data = (uint8_t *)pkt_data + vhdr->csum_start;

    len = pkt_len - vhdr->csum_start;

    /* Put the checksum obtained into the packet */

    stw_be_p(data + vhdr->csum_offset, net_raw_checksum(data, len));



    vhdr->flags &= ~VIRTIO_NET_HDR_F_NEEDS_CSUM;

    vhdr->flags |= VIRTIO_NET_HDR_F_DATA_VALID;

}
