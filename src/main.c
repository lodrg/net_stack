#define _GNU_SOURCE
#include "basic.h"
#include "tun.h"
#include "hdrs.h"
#include "netdev.h"
#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include "arp.h"
#include "ipv4.h"

#define BUF_SIZE 100

typedef void (*signal_handler_t)(int);

int running = 1;

int _signal(int signo, signal_handler_t handler) {
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_flags |= SA_RESTART;
    sa.sa_handler = handler;

    if (sigaction(signo, &sa, NULL) == -1) {
        return -1;
    }
    return 0;
}

static void stop_stack_handler(int signo) {
    (void)signo;
    running = 0;
}

void init_signals()
{
    _signal(SIGINT, stop_stack_handler);
}

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

void cleanup_all(struct netdev *netdev)
{
    printf("\n[-] Stack shutting down...\n");
    tun_close();
    netdev_cleanup(&netdev);
    arp_cleanup();
}

// ---------------- 主逻辑 ----------------
int main() {
    char buf[BUF_SIZE];
    CLEAR(buf);
    struct netdev netdev;
    char ifname[IFNAMSIZ] = "tap0";

    init_signals();

    // 初始化 TUN/TAP 设备
    tun_init(ifname);

    printf("[+] TAP device %s initialized\n", ifname);

    netdev_init_str(&netdev,"10.0.0.4","00:0c:29:6d:50:25");

    // 初始化 ARP 模块
    arp_init();

    // 主循环
    while (running) {
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

    cleanup_all(&netdev);

    return 0;
}
