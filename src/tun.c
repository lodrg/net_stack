#include "include/basic.h"
#include "utils.h"

static int tun_fd;

// 分配并初始化 TUN/TAP 设备
int tun_alloc(char *dev) 
{
    struct ifreq ifr;
    int fd, err;

    // open the tun device file
    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("open /dev/net/tun");
        exit(EXIT_FAILURE);
    }

    // 初始化 ifr 结构体
    memset(&ifr, 0, sizeof(ifr));
    // ifr 内容初始化
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    // 设备名
    if (*dev) strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    // 创建 TUN/TAP 设备
    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("ioctl TUNSETIFF");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // 返回设备名
    strcpy(dev, ifr.ifr_name);
    return fd;
}

// 设置接口路由
static int set_if_route(char *dev, char *cidr) 
{
    if (!dev || !cidr) {
        fprintf(stderr, "Invalid arguments: dev or cidr is NULL\n");
        return -1;
    }
    return run_cmd("ip route add %s dev %s", cidr, dev);
}

// 设置接口地址
static int set_if_addr(char *dev, char *addr) 
{
    if (!dev || !addr) {
        fprintf(stderr, "Invalid arguments: dev or addr is NULL\n");
        return -1;
    }
    return run_cmd("ip addr add %s dev %s", addr, dev);
}

// 启用接口
static int set_if_up(char *dev) 
{
    if (!dev) {
        fprintf(stderr, "Invalid arguments: dev or cidr is NULL\n");
        return -1;
    }
    return run_cmd("ip link set dev %s up", dev);
}

// 从 TUN/TAP 设备读取数据
int tun_read(char *buffer, int len) 
{
    return read(tun_fd, buffer, len);
}

// 向 TUN/TAP 设备写入数据
int tun_write(char *buffer, int len) 
{
    return write(tun_fd, buffer, len);
}

// 初始化 TUN/TAP 设备
void tun_init(char *dev_name) 
{
    tun_fd = tun_alloc(dev_name); 

    if (set_if_up(dev_name) != 0) {
        print_error("ERROR when setting up if\n");
    }
    if (set_if_addr(dev_name,"10.0.0.4") != 0) {
        print_error("ERROR when setting addr for if\n");
    }
    if (set_if_route(dev_name, "10.0.0.0/24") != 0) {
        print_error("ERROR when setting route for if\n");
    }
}

void tun_close()
{
    if (tun_fd >= 0) {
        close(tun_fd);
        tun_fd = -1;
    }
}   