#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>

#define BUF_SIZE 1600

struct eth_hdr {
    unsigned char dmac[6];
    unsigned char smac[6];
    uint16_t ethertype;
    unsigned char payload[];
} __attribute__((packed));

struct arp_hdr {
    uint16_t hwtype;
    uint16_t protype;
    unsigned char hwsize;
    unsigned char prosize;
    uint16_t opcode;
    unsigned char data[];
} __attribute__((packed));

struct arp_ipv4 {
    unsigned char smac[6];
    uint32_t sip;
    unsigned char dmac[6];
    uint32_t dip;
} __attribute__((packed));

int tun_alloc(char *dev) {
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("open /dev/net/tun");
        exit(1);
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (*dev) strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("ioctl TUNSETIFF");
        close(fd);
        exit(1);
    }

    strcpy(dev, ifr.ifr_name);
    return fd;
}

// ---------------- 主逻辑 ----------------
int main() {
    char ifname[IFNAMSIZ] = "tap0";
    int fd = tun_alloc(ifname);
    printf("[+] TAP device %s initialized\n", ifname);

    // 我们虚构的 "本机 IP+MAC"
    uint32_t my_ip = inet_addr("10.0.0.4");   // 注意: 用 network byte order
    unsigned char my_mac[6] = {0x00,0x0c,0x29,0x6d,0x50,0x25};

    char buf[BUF_SIZE];
    while (1) {
        int nread = read(fd, buf, sizeof(buf));
        if (nread < 0) {
            perror("read");
            continue;
        }

        struct eth_hdr *eth = (struct eth_hdr*) buf;
        if (ntohs(eth->ethertype) == ETH_P_ARP) {
            struct arp_hdr *arp = (struct arp_hdr*) eth->payload;
            struct arp_ipv4 *arpp = (struct arp_ipv4*) arp->data;

            if (ntohs(arp->opcode) == 1 && arpp->dip == my_ip) {
                printf("[+] Got ARP request for my IP. Sending reply...\n");

                // 构造 ARP reply
                memcpy(eth->dmac, eth->smac, 6);      // 目标 = 请求者MAC
                memcpy(eth->smac, my_mac, 6);         // 源 = 我的MAC

                arp->opcode = htons(2);               // Reply
                memcpy(arpp->dmac, arpp->smac, 6);    // 请求者的 MAC
                arpp->dip = arpp->sip;                // 请求者的 IP
                memcpy(arpp->smac, my_mac, 6);        // 我的 MAC
                arpp->sip = my_ip;                    // 我的 IP

                // 发回
                write(fd, buf, nread);
            }
        }
    }
}