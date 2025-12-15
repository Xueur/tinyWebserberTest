#include <unistd.h>
#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(EventLoop* _loop, int _fd) 
    : evlp(_loop), fd(_fd), events(0), ready(0), inEpoll(false), useThreadPool(true) {

}

Channel::~Channel() {
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

void Channel::handleEvent() {
    if(ready & (EPOLLIN | EPOLLPRI)){
        if(useThreadPool)       
            evlp->addThread(readCallback);
        else
            readCallback();
    }
    if(ready & (EPOLLOUT)){
        if(useThreadPool)       
            evlp->addThread(writeCallback);
        else
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

void Channel::setUseThreadPool(bool use = true) {
    useThreadPool = use;
}