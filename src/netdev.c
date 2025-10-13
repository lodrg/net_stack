#include "netdev.h"
#include <string.h>
#include "tun.h"
#include "hdrs.h"
#include <arpa/inet.h>

// 初始化网络设备
void netdev_init(struct netdev *ndev, uint32_t ip, unsigned char *mac)
{
    if (!ndev || !mac) {
        return;
    }
    // 初始化 netdev 结构体
    // 注意: 这里没有做任何错误检查
    ndev->ip = ip;
    memcpy(ndev->mac, mac, 6);
}

// 发送以太网帧
// 参数: dev: 网络设备
//      hdr: 以太网帧头
//      ethertype: 以太网类型
//      len: 负载长度
//      dst: 目的 MAC 地址
void netdev_transmit(struct netdev *dev, struct eth_hdr *hdr, 
                     uint16_t ethertype, int len, unsigned char *dst)
{
    hdr->ethertype = htons(ethertype);

    memcpy(hdr->smac, dev->mac, 6);
    memcpy(hdr->dmac, dst, 6);

    len += sizeof(struct eth_hdr);

    tun_write((char *)hdr, len);
}
