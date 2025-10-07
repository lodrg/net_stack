#include "netdev.h"
#include <string.h>

void netdev_init(struct netdev *ndev, uint32_t ip, unsigned char *mac)
{
    if (!ndev || !mac) {
        return;
    }
    ndev->ip = ip;
    memcpy(ndev->mac, mac, 6);
}