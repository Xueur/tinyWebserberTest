#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(EventLoop* _loop, int _fd) : evlp(_loop), fd(_fd), events(0), revents(0), inEpoll(false) {

}

Channel::~Channel() {

}

void Channel::handleEvent() {
    callback();
}


void Channel::enableReading() {
    events = EPOLLIN | EPOLLET;
    evlp->updateChannel(this);
}

int Channel::getFd() {
    return fd;
}

uint32_t Channel::getEvents() {
    return events;
}

uint32_t Channel::getRevents() {
    return revents;
}

void Channel::setRevents(uint32_t ev) {
    revents = ev;
}

bool Channel::isInEpoll() {
    return inEpoll;
}

void Channel::setInEpoll() {
    inEpoll = true;
}

void Channel::setCallBack(std::function<void()> cb) {
    callback = cb;
}