#pragma once
#include <sys/epoll.h>
#include <functional>

class EventLoop;
class Channel {
private:
    EventLoop* evlp;
    int fd;
    uint32_t events;
    uint32_t ready;
    bool inEpoll;
    std::function<void()> readCallback;
    std::function<void()> writeCallback;
public:
    Channel(EventLoop*, int);
    ~Channel();
    void handleEvent();
    void enableReading();
    int getFd();
    uint32_t getEvents();
    uint32_t getReady();
    bool isInEpoll();
    void setInEpoll(bool);
    void useET();

    void setReady(uint32_t);
    void setReadCallback(std::function<void()>);
};

