#pragma once

int tun_alloc(char *dev); 
int tun_read(char *buffer, int len);
int tun_write(char *buffer, int len);
void tun_init(char *dev_name);
void tun_close();