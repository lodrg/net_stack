#include "ipv4.h"
#include "icmpv4.h"

void ipv4_incoming(struct netdev *netdev, struct eth_hdr *hdr)
{
    struct iphdr *iphdr = (struct iphdr *)hdr->payload;
    uint16_t csum = -1;

    if (iphdr->version != IPV4) {
        printf("IPv4: Not IPv4 packet\n");
        return;
    }

    if (iphdr->ihl >= 5) {
        printf("IPv4: Header length %d bytes\n", iphdr->ihl * 4);
        return;
    }

    if (iphdr->len == 0) {
        // TODO: Send ICMP error message
        printf("IPv4: Invalid length\n");
        return;
    }

    csum = checksum((uint16_t *)iphdr, iphdr->ihl * 4);
    
    if (csum != 0) {
        printf("IPv4: Invalid checksum\n");
        return;
    }

    iphdr->len = ntohs(iphdr->len);

    switch (iphdr->proto) {
        case ICMPV4:
            printf("IPv4: ICMPv4 packet\n");
            icmpv4_incoming(netdev, hdr);
            break;
        default:
            printf("IPv4: Unrecognized protocol %d\n", iphdr->proto);
            break;
    }
}