#pragma once
#include <arpa/inet.h>

class InetAddress {
public:
    struct sockaddr_in addr;
    socklen_t addr_len;
    InetAddress();
    InetAddress(const char* id, uint16_t port);
    ~InetAddress();
};