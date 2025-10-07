#include "include/basic.h"
#include "utils.h"

static int tun_fd;

int tun_alloc(char *dev) 
{
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("open /dev/net/tun");
        exit(EXIT_FAILURE);
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (*dev) strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("ioctl TUNSETIFF");
        close(fd);
        exit(EXIT_FAILURE);
    }

    strcpy(dev, ifr.ifr_name);
    return fd;
}

static int set_if_route(char *dev, char *cidr) 
{
    if (!dev || !cidr) {
        fprintf(stderr, "Invalid arguments: dev or cidr is NULL\n");
        return -1;
    }
    return run_cmd("ip route add %s dev %s", cidr, dev);
}

static int set_if_addr(char *dev, char *addr) 
{
    if (!dev || !addr) {
        fprintf(stderr, "Invalid arguments: dev or addr is NULL\n");
        return -1;
    }
    return run_cmd("ip addr add %s dev %s", addr, dev);
}

static int set_if_up(char *dev) 
{
    if (!dev) {
        fprintf(stderr, "Invalid arguments: dev or cidr is NULL\n");
        return -1;
    }
    return run_cmd("ip link set dev %s up", dev);
}

int tun_read(char *buffer, int len) 
{
    return read(tun_fd, buffer, len);
}

int tun_write(char *buffer, int len) 
{
    return write(tun_fd, buffer, len);
}

void tun_init(char *dev) 
{
    tun_fd = tun_alloc(dev); 

    if (set_if_up(dev) != 0) {
        print_error("ERROR when setting up if\n");
    }
    if (set_if_addr(dev,"10.0.0.4") != 0) {
        print_error("ERROR when setting addr for if\n");
    }
    if (set_if_route(dev, "10.0.0.0/24") != 0) {
        print_error("ERROR when setting route for if\n");
    }
}