### 在另一个终端配置tap0 设备
sudo ip link set tap0 up
sudo ip addr add 10.0.0.1/24 dev tap0

上面的命令已经在初始化中实现

### 测试程序是否工作

arping -I tap0 10.0.0.4

### 清除内核持有的 IP
sudo ip addr flush dev tap0
sudo ip link set tap0 up

### ping 对应的 IP 通过对应的网口
ping -I tap0 10.0.0.4