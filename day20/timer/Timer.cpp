#include "Timer.h"

Timer::Timer(const TimeStamp& timeStamp, std::function<void()> cb, double intervalArg)
    : expiration(timeStamp),
      callback(std::move(cb)),
      interval(intervalArg),
      repeat(intervalArg > 0.0) {}

void Timer::reStart(const TimeStamp& now) {
    expiration = TimeStamp::addTime(now, interval);
}

void Timer::run() const {
    if (callback) {
        callback();
    }
}

TimeStamp Timer::getExpiration() const {
    return expiration;
}

bool Timer::isRepeat() const {
    return repeat;
}

