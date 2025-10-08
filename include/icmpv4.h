#pragma once
#include "netdev.h"
#include "hdrs.h"

void icmpv4_incoming(struct netdev *netdev, struct eth_hdr *hdr);
void icmpv4_outgoing(struct netdev *netdev, struct eth_hdr *hdr);