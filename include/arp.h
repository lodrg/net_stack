#pragma once
#include "netdev.h"
#include "hdrs.h"

#define ARP_ETHERNET    0x0001
#define ARP_IPV4        0x0800
#define ARP_REQUEST     0x0001
#define ARP_REPLY       0x0002

#define ARP_CACHE_LEN   32
#define ARP_FREE        0
#define ARP_WAITING     1
#define ARP_RESOLVED    2

struct arp_cache_entry
{
    uint16_t hwtype;
    uint32_t sip;
    unsigned char smac[6];
    unsigned int state;
};

void arp_init();
void arp_incoming(struct netdev *netdev, struct eth_hdr *hdr);
void arp_reply(struct netdev *netdev, struct eth_hdr *hdr, struct arp_hdr *arphdr);
