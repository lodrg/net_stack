### 在另一个终端配置tap0 设备
sudo ip link set tap0 up
sudo ip addr add 10.0.0.1/24 dev tap0

### 测试程序是否工作
ping 10.0.0.4

arping -I tap0 10.0.0.4


