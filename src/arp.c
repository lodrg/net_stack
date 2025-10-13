#include "arp.h"
#include "basic.h"
#include "hdrs.h"
#include <netinet/in.h>
#include <stdio.h>


// ARP 缓存表
// 注意: 这里没有做过期处理
static struct arp_cache_entry arp_cache[ARP_CACHE_LEN];

// 更新 ARP 缓存表
// 如果找到已有的条目, 则更新并返回 1
// 否则返回 0
static int update_arp_translation_table(struct arp_hdr *hdr, struct arp_ipv4 *data)
{
     struct arp_cache_entry *entry;

    for (int i = 0; i<ARP_CACHE_LEN; i++) {
        entry = &arp_cache[i];

        if (entry->state == ARP_FREE) continue;

        // 找到已有条目, 更新
        if (entry->hwtype == hdr->hwtype && entry->sip == data->sip) {
            memcpy(entry->smac, data->smac, 6);
            return 1;
        }
    }
    
    return 0;
}

// 插入 ARP 缓存表
// 如果成功插入则返回 0
// 否则返回 -1
static int insert_arp_translation_table(struct arp_hdr *hdr, struct arp_ipv4 *data)
{
    struct arp_cache_entry *entry;
    for (int i = 0; i<ARP_CACHE_LEN; i++) {
        entry = &arp_cache[i];

        if (entry->state == ARP_FREE) {
            entry->state = ARP_RESOLVED;

            entry->hwtype = hdr->hwtype;
            entry->sip = data->sip;
            memcpy(entry->smac, data->smac, sizeof(entry->smac));

            return 0;
        }
    }

    return -1;
}

// 初始化 ARP 模块
void arp_init()
{
    memset(arp_cache, 0, ARP_CACHE_LEN * sizeof(struct arp_cache_entry));
}

// 处理收到的 ARP 包
void arp_incoming(struct netdev *netdev, struct eth_hdr *hdr)
{
    struct arp_hdr *arphdr = (struct arp_hdr *)hdr->payload;
    struct arp_ipv4 *arpdata = (struct arp_ipv4 *)arphdr->data;
    int merge = 0;

    // 获得 ARP 头部各字段的设备类型 和协议类型 以及操作码
    arphdr->hwtype = ntohs(arphdr->hwtype);
    arphdr->protype = ntohs(arphdr->protype);
    arphdr->opcode = ntohs(arphdr->opcode);

    // 只处理 Ethernet + IPv4 的 ARP 包
    if (arphdr->hwtype != ARP_ETHERNET || arphdr->protype != ARP_IPV4 ||
        arphdr->hwsize != 6 || arphdr->prosize != 4) {
        printf("ARP: Unknown hardware or protocol type\n");
        return;
    }

    // 更新 ARP 缓存表
    merge = update_arp_translation_table(arphdr, arpdata);

    if (netdev->ip != arpdata->dip) {
        printf("ARP: Not for us\n ip=%x dip=%x\n", ntohl(netdev->ip), ntohl(arpdata->dip));
    }

    // 如果没有找到已有条目, 则插入新条目
    if (!merge && insert_arp_translation_table(arphdr, arpdata) != 0) {
       perror("ERR: No free space in ARP translation table\n"); 
    }

    // 处理 ARP 请求和应答
    switch (arphdr->opcode) {
        case ARP_REQUEST:
            printf("ARP: Request\n");
            arp_reply(netdev, hdr, arphdr);
            break;
        case ARP_REPLY:
            printf("ARP: Reply\n");
            break;
        default:
            printf("ARP: Unknown opcode %d\n", arphdr->opcode);
            break;
    }
}

// 发送 ARP 应答
// 参数: netdev: 网络设备
//      hdr: 以太网帧头
//      arphdr: ARP 头
void arp_reply(struct netdev *netdev, struct eth_hdr *hdr, struct arp_hdr *arphdr)
{
    struct arp_ipv4 *arpdata;
    int len;

    // 构造 ARP 应答
    arpdata = (struct arp_ipv4 *) arphdr->data;

    memcpy(arpdata->dmac, arpdata->smac, 6);
    arpdata->dip = arpdata->sip;
    memcpy(arpdata->smac, netdev->mac, 6);
    arpdata->sip = netdev->ip;

    arphdr->opcode = ARP_REPLY;

    arphdr->opcode = htons(arphdr->opcode);
    arphdr->hwtype = htons(arphdr->hwtype);
    arphdr->protype = htons(arphdr->protype);

    // 发送 ARP 应答
    len = sizeof(struct arp_hdr) + sizeof(struct arp_ipv4);
    netdev_transmit(netdev, hdr, ETH_P_ARP, len, arpdata->dmac);
}

