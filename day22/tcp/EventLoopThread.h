#pragma once
#include "common.h"
#include <mutex>
#include <thread>
#include <condition_variable>

class EventLoop;
class EventLoopThread{
    public:
        DISALLOW_COPY_AND_MOVE(EventLoopThread)
        EventLoopThread();
        ~EventLoopThread();

        EventLoop *StartLoop();

    private:
        void ThreadFunc();
        EventLoop *lp;
        std::thread thread;
        std::mutex mtx;
        std::condition_variable cv;
};