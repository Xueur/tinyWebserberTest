#include <unistd.h>
#include <sys/epoll.h>
#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(EventLoop* _loop, int _fd) 
    : evlp(_loop), fd(_fd), events(0), ready(0), inEpoll(false) {

}

Channel::~Channel() {
    if(fd != -1){
        close(fd);
        fd = -1;
    } 
}

void Channel::Tie(const std::shared_ptr<void> &ptr){
    isTied = true;
    tie = ptr;
}

void Channel::handleEvent() const{
    if(isTied){
        std::shared_ptr<void> guard = tie.lock();
        handleEventWithGuard();
    }else{
        handleEventWithGuard();
    }
}

void Channel::handleEventWithGuard() const {
    if(ready & (EPOLLIN | EPOLLPRI| EPOLLRDHUP)){
        if (readCallback) readCallback();
    }
    if(ready & (EPOLLOUT)){
        if (writeCallback) writeCallback();
    }
}

void Channel::enableReading() {
    events |= EPOLLIN | EPOLLPRI;
    evlp->updateChannel(this);
}

void Channel::enableWriting() {
    events |= EPOLLOUT;
    evlp->updateChannel(this);
}

void Channel::disableWriting() {
    events &= ~EPOLLOUT;
    evlp->updateChannel(this);
}

void Channel::useET() {
    events |= EPOLLET;
    evlp -> updateChannel(this);
}

int Channel::getFd() const {
    return fd;
}

uint32_t Channel::getEvents() const {
    return events;
}

uint32_t Channel::getReady() const {
    return ready;
}

bool Channel::isInEpoll() const {
    return inEpoll;
}

void Channel::setInEpoll(bool flag) {
    inEpoll = flag;
}

void Channel::setReady(uint32_t ev) {
    ready = ev;
}

void Channel::setReadCallback(std::function<void()> const& cb) {
    readCallback = std::move(cb);
}

void Channel::setWriteCallback(std::function<void()> const& cb) {
    writeCallback = std::move(cb);
}
