#include "TimerQueue.h"
#include "Timer.h"
#include "EventLoop.h"
#include "Channel.h"
#include <cstring>
#include <iostream>
#include <assert.h>

TimerQueue::TimerQueue(EventLoop* loop)
    : lp(loop),
      timerFd(-1),
      ch(nullptr) {
    createTimerFd();
    ch = std::make_unique<Channel>(lp, timerFd);
    ch->setReadCallback(std::bind(&TimerQueue::handleRead, this));
    ch->enableReading();
}

TimerQueue::~TimerQueue() {
    lp->deleteChannel(ch.get());
    ::close(timerFd);
    for (const auto& entry : timers) {
        delete entry.second;
    }
}

void TimerQueue::createTimerFd() {
    timerFd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerFd < 0) {
        printf("Failed to create timerfd: %s\n", strerror(errno));
    }
}

void TimerQueue::readTimerFd() {
    uint64_t expirations;
    ssize_t n = ::read(timerFd, &expirations, sizeof(expirations));
    if (n != sizeof(expirations)) {
        printf("Failed to read timerfd: %s\n", strerror(errno));
    }
}

void TimerQueue::handleRead() {
    readTimerFd();
    activeTimers.clear();
    auto end = timers.lower_bound(Entry(TimeStamp::now(), reinterpret_cast<Timer*>(UINTPTR_MAX)));
    activeTimers.insert(activeTimers.end(), timers.begin(), end);
    timers.erase(timers.begin(), end);
    for (const auto& entry : activeTimers) {
        entry.second->run();
    }
    resetTimers();
}

bool TimerQueue::InsertTimer(Timer* timer) {
    bool earliestChanged = false;
    TimeStamp when = timer->getExpiration();
    auto it = timers.begin();
    if (it == timers.end() || when < it->first) {
        earliestChanged = true;
    }
    timers.emplace(std::move(Entry(when, timer)));
    return earliestChanged;
}

void TimerQueue::addTimer(TimeStamp timestamp, std::function<void()> const & cb, double interval) {
    Timer* timer = new Timer(timestamp, cb, interval);
    if (InsertTimer(timer)) {
        resetTimerFd(timer);
    }
}

void TimerQueue::resetTimers() {
    TimeStamp now = TimeStamp::now();
    for (const auto& entry : activeTimers) {
        Timer* timer = entry.second;
        if (timer->isRepeat()) {
            timer->reStart(now);
            InsertTimer(timer);
        } else {
            delete timer;
        }
    }
    if (!timers.empty()) {
        resetTimerFd(timers.begin()->second);
    }
}

void TimerQueue::resetTimerFd(Timer* timer) {
    struct itimerspec new_;
    memset(&new_, '\0', sizeof(new_));

    int64_t micro_seconds_dif = timer->getExpiration().getMicroSecond() - TimeStamp::now().getMicroSecond();
    if (micro_seconds_dif < 100){
        micro_seconds_dif = 100;
    }

    new_.it_value.tv_sec = static_cast<time_t>(
        micro_seconds_dif / kMicrosecond2Second);
    new_.it_value.tv_nsec = static_cast<long>((
        micro_seconds_dif % kMicrosecond2Second) * 1000);
    // 获取监控的事件的时间距离当前有多久。

    int ret = ::timerfd_settime(timerFd, 0, &new_, nullptr);
    assert(ret != -1);
    (void)ret;
}