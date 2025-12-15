#pragma once
#include <sys/epoll.h>
#include <functional>

class EventLoop;
class Channel {
private:
    EventLoop* evlp;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inEpoll;
    std::function<void()> callback;
public:
    Channel(EventLoop*, int);
    ~Channel();
    void handleEvent();
    void enableReading();
    int getFd();
    uint32_t getEvents();
    void setRevents(uint32_t);
    uint32_t getRevents();
    bool isInEpoll();
    void setInEpoll();
    void setCallBack(std::function<void()>);
};

