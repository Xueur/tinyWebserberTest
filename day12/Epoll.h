#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;
class Epoll {
private:
    int epfd;
    struct epoll_event* events;
public:
    
    Epoll();
    ~Epoll();
    void addFd(int, uint32_t);
    void updateChannel(Channel*);
    void deleteChannel(Channel*);
    //std::vector<struct epoll_event> poll(int timeout);
    std::vector<Channel*> poll(int timeout = -1);
};