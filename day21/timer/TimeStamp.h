#pragma once
#include <string>
#include <sys/time.h>

const int kMicrosecond2Second = 1000 * 1000;

class TimeStamp {
public:
    TimeStamp() : microSecond(0) {}
    explicit TimeStamp(int64_t microSecondArg) : microSecond(microSecondArg) {}
    bool operator<(const TimeStamp& rhs) const {
        return microSecond < rhs.microSecond;
    }
    bool operator==(const TimeStamp& rhs) const {
        return microSecond == rhs.microSecond;
    }
    std::string toString() const {
        char buf[64] = {0};
        time_t seconds = static_cast<time_t>(microSecond / kMicrosecond2Second);
        struct tm tm_time;
        localtime_r(&seconds, &tm_time);
        int microseconds = static_cast<int>(microSecond % kMicrosecond2Second);
        snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d.%06d",
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds);
        return buf;
    }
    int64_t getMicroSecond() const {
        return microSecond;
    }
    static TimeStamp now();
    static TimeStamp addTime(const TimeStamp& timestamp, double seconds);
private:
    int64_t microSecond;
};

inline TimeStamp TimeStamp::now() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t microSecond = static_cast<int64_t>(tv.tv_sec) * kMicrosecond2Second + tv.tv_usec;
    return TimeStamp(microSecond);
}

inline TimeStamp TimeStamp::addTime(const TimeStamp& timestamp, double seconds) {
    int64_t delta = static_cast<int64_t>(seconds * kMicrosecond2Second);
    return TimeStamp(timestamp.getMicroSecond() + delta);
}
