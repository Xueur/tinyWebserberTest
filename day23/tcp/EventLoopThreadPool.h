#pragma once

#include "common.h"
#include <memory>
#include <thread>
#include <vector>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
    DISALLOW_COPY_AND_MOVE(EventLoopThreadPool);
    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();
    void setThreadPoolSize(int num);
    void Start();
    EventLoop* getNextLoop();
private:
    EventLoop* main_reactor;
    int next;
    int size;
    std::vector<std::unique_ptr<EventLoopThread>> threads;
    std::vector<EventLoop*> loops;
};
