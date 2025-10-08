#include <stdio.h>
#include "icmpv4.h"

void icmpv4_incoming(struct netdev *netdev, struct eth_hdr *hdr)
{
    printf("ICMPv4: Incoming packet\n");
}
void icmpv4_outgoing(struct netdev *netdev, struct eth_hdr *hdr)
{
    printf("ICMPv4: Outgoing packet\n");
}