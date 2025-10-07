#pragma once
#include <stdint.h>

struct netdev 
{
    uint32_t ip;
    unsigned char mac[6];
};

void netdev_init(struct netdev *ndev, uint32_t ip, unsigned char *mac);