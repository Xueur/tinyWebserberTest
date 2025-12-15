#pragma once
#include <sys/epoll.h>
#include <vector>

class Epoll {
private:
    int epfd;
    struct epoll_event* events;
public:
    Epoll();
    ~Epoll();
    void addFd(int, uint32_t);
    std::vector<struct epoll_event> poll(int timeout);
};