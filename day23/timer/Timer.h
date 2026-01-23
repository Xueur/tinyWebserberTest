#pragma once
#include <functional>
#include "TimeStamp.h"
#include "common.h"

class Timer {
public:
    DISALLOW_COPY_AND_MOVE(Timer);
    Timer(const TimeStamp& timeStamp, std::function<void()> cb, double interval);
    void reStart(const TimeStamp& now);
    void run () const;
    TimeStamp getExpiration() const;
    bool isRepeat() const;
private:
    TimeStamp expiration;
    std::function<void()> callback;
    double interval;
    bool repeat;
};