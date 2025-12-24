#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
    : main_reactor(baseLoop), next(0), size(0) {
}

EventLoopThreadPool::~EventLoopThreadPool() {
}

void EventLoopThreadPool::setThreadPoolSize(int num) {
    size = num;
}

void EventLoopThreadPool::Start() {
    for (int i = 0; i < size; ++i) {
        auto thread = std::make_unique<EventLoopThread>();
        loops.emplace_back(thread->StartLoop());
        threads.emplace_back(std::move(thread));
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    EventLoop* loop = main_reactor;
    if (!loops.empty()) {
        loop = loops[next];
        next = (next + 1) % loops.size();
    }
    return loop;
}