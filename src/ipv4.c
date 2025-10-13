#include "ipv4.h"
#include "icmpv4.h"

// 处理收到的 IPv4 包
// 参数: netdev: 网络设备
//      hdr: 以太网帧头 
void ipv4_incoming(struct netdev *netdev, struct eth_hdr *hdr)
{
    struct iphdr *iphdr = (struct iphdr *)hdr->payload;
    uint16_t csum = -1;

    // 验证 IPv4 头部各字段
    if (iphdr->version != IPV4) {
        printf("IPv4: Not IPv4 packet\n");
        return;
    }

    // 头部长度至少 5 个 32-bit 字
    if (iphdr->ihl >= 5) {
        printf("IPv4: Header length %d bytes\n", iphdr->ihl * 4);
        return;
    }

    // 总长度至少要大于头部长度
    if (iphdr->len == 0) {
        // TODO: Send ICMP error message
        printf("IPv4: Invalid length\n");
        return;
    }

    // 计算并验证校验和
    csum = checksum((uint16_t *)iphdr, iphdr->ihl * 4);
    
    if (csum != 0) {
        printf("IPv4: Invalid checksum\n");
        return;
    }

    // 获得总长度, 转为主机字节序
    iphdr->len = ntohs(iphdr->len);

    // 处理不同的协议
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