#pragma once
#include <functional>
#include <memory>
#include "common.h"

class Epoll;
class EventLoop {
private:
    std::unique_ptr<Epoll> ep;
    bool quit;
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void updateChannel(Channel*);
    void deleteChannel(Channel*);
};