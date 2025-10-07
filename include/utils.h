#pragma once
#define CMDBUFLEN 100

int run_cmd(char *cmd, ...);
void print_hexdump(char *str, int len);
void print_error(char *str, ...);