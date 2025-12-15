#include "InetAddress.h"
#include <string.h>

InetAddress::InetAddress() : addr_len(sizeof(addr)) {
    bzero(&addr, addr_len);
}

InetAddress::InetAddress(const char* id, uint16_t port) : addr_len(sizeof(addr)) {
    bzero(&addr, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(id);
    addr.sin_port = htons(port);
}

InetAddress::~InetAddress() {}

