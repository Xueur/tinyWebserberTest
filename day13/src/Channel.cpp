#include <unistd.h>
#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(EventLoop* _loop, int _fd) 
    : evlp(_loop), fd(_fd), events(0), ready(0), inEpoll(false) {

}

Channel::~Channel() {
    // 不在这里关闭fd，由Socket负责
    // Channel只是对fd的包装，不拥有所有权
}

void Channel::handleEvent() {
    if(ready & (EPOLLIN | EPOLLPRI)){
        readCallback();
    }
    if(ready & (EPOLLOUT)){
        writeCallback();
    }
}


void Channel::enableReading() {
    events |= EPOLLIN | EPOLLPRI;
    evlp->updateChannel(this);
}


void Channel::useET() {
    events |= EPOLLET;
    evlp -> updateChannel(this);
}

int Channel::getFd() {
    return fd;
}

uint32_t Channel::getEvents() {
    return events;
}

uint32_t Channel::getReady() {
    return ready;
}


void Channel::setReady(uint32_t ev) {
    ready = ev;
}

bool Channel::isInEpoll() {
    return inEpoll;
}

void Channel::setInEpoll(bool flag) {
    inEpoll = flag;
}

void Channel::setReadCallback(std::function<void()> cb) {
    readCallback = cb;
}
