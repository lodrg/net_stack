#include "netdev.h"
#include <string.h>
#include "tun.h"
#include "hdrs.h"
#include <arpa/inet.h>

void netdev_init(struct netdev *ndev, uint32_t ip, unsigned char *mac)
{
    if (!ndev || !mac) {
        return;
    }
    ndev->ip = ip;
    memcpy(ndev->mac, mac, 6);
}

void netdev_transmit(struct netdev *dev, struct eth_hdr *hdr, 
                     uint16_t ethertype, int len, unsigned char *dst)
{
    hdr->ethertype = htons(ethertype);

    memcpy(hdr->smac, dev->mac, 6);
    memcpy(hdr->dmac, dst, 6);

    len += sizeof(struct eth_hdr);

    tun_write((char *)hdr, len);
}
