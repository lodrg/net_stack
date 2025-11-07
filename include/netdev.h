#pragma once
#include <stdint.h>
#include "hdrs.h"

struct netdev 
{
    uint32_t ip;
    unsigned char mac[6];
};

void netdev_init_bin(struct netdev *ndev, uint32_t ip, unsigned char *mac);

void netdev_init_str(struct netdev *ndev, const char *ip_str, const char *mac_str);

void netdev_transmit(struct netdev *dev, struct eth_hdr *hdr, uint16_t ethertype, int len, unsigned char *dst);