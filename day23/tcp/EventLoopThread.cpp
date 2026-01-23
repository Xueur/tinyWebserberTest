#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread() : lp(nullptr){}

EventLoopThread::~EventLoopThread(){}

EventLoop* EventLoopThread::StartLoop(){
    thread = std::thread(&EventLoopThread::ThreadFunc, this);
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() {return lp != nullptr;});
    }
    return lp;
}

void EventLoopThread::ThreadFunc(){
    EventLoop loop;
    {
        std::unique_lock<std::mutex> lock(mtx);
        lp = &loop;
        cv.notify_one();
    }
    loop.loop();
}