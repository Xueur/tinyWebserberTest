#pragma once
#include <unistd.h>
#include <sys/timerfd.h>
#include <set>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include "common.h"
#include "TimeStamp.h"

class Timer;
class EventLoop;
class Channel;
class TimerQueue {
public:
    DISALLOW_COPY_AND_MOVE(TimerQueue);
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();
    void createTimerFd();
    void handleRead();
    void readTimerFd();
    void resetTimerFd(Timer* timer);
    void resetTimers();
    bool InsertTimer(Timer* timer);
    void addTimer(TimeStamp timestamp, std::function<void()> const & cb, double interval);
private:
    typedef std::pair<TimeStamp, Timer *> Entry;
    EventLoop* lp;
    int timerFd;
    std::unique_ptr<Channel> ch; 
    std::set<Entry> timers; // 定时器集合
    std::vector<Entry> activeTimers; // 激活的定时器
};