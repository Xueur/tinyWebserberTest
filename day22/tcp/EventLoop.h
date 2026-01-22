#pragma once
#include <memory>
#include <mutex>
#include <functional>
#include <vector>
#include <thread>
#include "common.h"
#include "TimeStamp.h"

class Epoll;
class Channel;
class TimerQueue;
class EventLoop {
public:
    DISALLOW_COPY_AND_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();
    void loop();
    void updateChannel(Channel*);
    void deleteChannel(Channel*);
    void DoToDoList();
    void QueueOneFunc(std::function<void()> fn); 
    void RunOneFunc(std::function<void()> fn);
    bool IsInLoopThread();
    void HandleRead();

    // 定时器功能
    void RunAt(TimeStamp timestamp, std::function<void()> const & cb);
    void RunAfter(double wait_time, std::function < void()>const & cb);
    void RunEvery(double interval, std::function<void()> const & cb);
private:
    std::unique_ptr<Epoll> ep;
    bool quit;
    std::vector<std::function<void()>> to_do_list;
    std::mutex mtx;
    int wakeup_fd;
    std::unique_ptr<Channel> wakeup_ch;
    bool calling_functors;
    pid_t tid;
    std::unique_ptr<TimerQueue> timer_queue;
};