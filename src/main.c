#include "basic.h"
#include "tun.h"
#include "hdrs.h"
#include "netdev.h"
#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include "arp.h"
#include "ipv4.h"

#define BUF_SIZE 1600

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
    tun_init(ifname);

    printf("[+] TAP device %s initialized\n", ifname);

    struct netdev netdev;
    // 我们虚构的 "本机 IP+MAC"
    uint32_t my_ip = inet_addr("10.0.0.4");   // 注意: 用 network byte order
    unsigned char my_mac[6] = {0x00,0x0c,0x29,0x6d,0x50,0x25};

    netdev_init(&netdev,my_ip,my_mac);

    // arp_init();

    char buf[BUF_SIZE];

    while (1) {
        int nread = tun_read(buf, sizeof(buf));
        if (nread < 0) {
            print_error("ERR: Read from tun_fd: %s\n", strerror(errno));
        }

        // print_hexdump(buf, BUF_SIZE);

        struct eth_hdr *eth = (struct eth_hdr*) buf;
        eth->ethertype = ntohs(eth->ethertype);
        handle_frame(&netdev, eth);
    }
}