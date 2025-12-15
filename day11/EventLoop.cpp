#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "ThreadPool.h"
#include <vector>

EventLoop::EventLoop() : quit(false) {
    ep = new Epoll();
    thp = new ThreadPool(16);
}

EventLoop::~EventLoop() {
    delete ep;
    delete thp;
}

void EventLoop::loop() {
    while (!quit) {
        std::vector<Channel*>chs = ep->poll();
        for (auto ch : chs) {
            ch->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel* ch) {
    ep->updateChannel(ch);
}

void EventLoop::addThread(std::function<void()> func) {
    thp->add(func);
}
