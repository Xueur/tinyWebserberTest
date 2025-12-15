#pragma once

class InetAddress;
class Socket {
private:
    int sockfd;
public:
    Socket();
    Socket(int);
    ~Socket();
    void bind(InetAddress*);
    void listen();
    void setnonblocking();
    int accept(InetAddress*);
    int getFd();
};