#include "basic.h"
#include "tun.h"
#include "hdrs.h"
#include "netdev.h"
#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include "arp.h"
#include "ipv4.h"

#define BUF_SIZE 100



void handle_frame(struct netdev *netdev, struct eth_hdr *hdr)
{
    switch (hdr->ethertype) {
        case ETH_P_ARP:
            arp_incoming(netdev, hdr);
            // (void)netdev; // 防止编译器警告
            printf("Found ARP\n");
            break;
        case ETH_P_IP:
            ipv4_incoming(netdev, hdr);
            printf("Found IPv4\n");
            break;
        default:
            printf("Unrecognized ethertype %x\n", hdr->ethertype);
            break;
    }
}

// ---------------- 主逻辑 ----------------
int main() {
    char ifname[IFNAMSIZ] = "tap0";
    // 初始化 TUN/TAP 设备
    tun_init(ifname);

    printf("[+] TAP device %s initialized\n", ifname);

    // 初始化 netdev 结构体
    struct netdev netdev;
    netdev_init_str(&netdev,"10.0.0.4","00:0c:29:6d:50:25");

    // 初始化 ARP 模块
    arp_init();

    char buf[BUF_SIZE];
    CLEAR(buf);

    // 主循环
    while (1) {
        // 从 TUN/TAP 设备读取数据
        int nread = tun_read(buf, sizeof(buf));
        if (nread < 0) {
            print_error("ERR: Read from tun_fd: %s\n", strerror(errno));
        }

        // print_hexdump(buf, BUF_SIZE);

        struct eth_hdr *eth = (struct eth_hdr*) buf;
        // 网络序（大端序）转换为主机序
        // 接下来会在主机中对以太网帧进行处理
        eth->ethertype = ntohs(eth->ethertype);
        // 处理以太网帧
        handle_frame(&netdev, eth);
    }
    free(ifname);
}