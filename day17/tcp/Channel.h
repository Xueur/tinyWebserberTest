#pragma once
#include "common.h"
#include <functional>
#include <memory>

class EventLoop;
class Channel {
public:
    DISALLOW_COPY_AND_MOVE(Channel);
    Channel(EventLoop*, int);
    ~Channel();

    void handleEvent() const;
    void handleEventWithGuard() const;
    void Tie(const std::shared_ptr<void> &ptr);
    
    void enableReading();
    void enableWriting();
    void disableWriting();
    void useET();

    int getFd() const;
    uint32_t getEvents() const;
    uint32_t getReady() const;
    bool isInEpoll() const;

    void setInEpoll(bool);
    void setReady(uint32_t);
    void setReadCallback(std::function<void()> const& cb);
    void setWriteCallback(std::function<void()> const& cb);
private:
    EventLoop* evlp;
    int fd;
    uint32_t events;
    uint32_t ready;
    bool inEpoll;

    bool isTied;
    std::weak_ptr<void> tie;

    std::function<void()> readCallback;
    std::function<void()> writeCallback;
};

