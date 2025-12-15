#pragma once
#include <arpa/inet.h>

class InetAddress {
private:
    struct sockaddr_in addr;
    socklen_t addr_len;
public:
    InetAddress();
    InetAddress(const char* id, uint16_t port);
    ~InetAddress();

    void setInetAddr(sockaddr_in _addr);
    sockaddr_in getAddr();
};