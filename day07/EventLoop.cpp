#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include <vector>

EventLoop::EventLoop() : quit(false) {
    ep = new Epoll();
}

EventLoop::~EventLoop() {
    delete ep;
}

void EventLoop::loop() {
    while (!quit) {
        std::vector<Channel*>chs = ep->poll(-1);
        for (auto ch : chs) {
            ch->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel* ch) {
    ep->updateChannel(ch);
}
