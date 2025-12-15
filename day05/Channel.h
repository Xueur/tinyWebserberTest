#pragma once
#include <sys/epoll.h>

class Epoll;
class Channel {
private:
    Epoll* ep;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inEpoll;
public:
    Channel(Epoll*, int);
    ~Channel();
    void enableReading();
    int getFd();
    uint32_t getEvents();
    void setRevents(uint32_t);
    uint32_t getRevents();
    bool isInEpoll();
    void setInEpoll(bool);
};

