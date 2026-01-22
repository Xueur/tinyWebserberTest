#include "Epoll.h"
#include "Channel.h"
#include "util.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1024 
Epoll::Epoll(): epfd(-1), events(nullptr) {
    epfd = epoll_create1(0);
    errif(epfd < 0, "epoll create error");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(struct epoll_event) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (epfd >= 0) {
        close(epfd);
        epfd = -1;
    }
    delete[] events;
}

void Epoll::updateChannel(Channel* ch) const {
    int fd = ch->getFd();
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = ch;
    ev.events = ch->getEvents();
    if (ch->isInEpoll()) {
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) < 0, "epoll mod fd error");
    } else {
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) < 0, "epoll add fd error");
        ch->setInEpoll(true);
    }
}

void Epoll::deleteChannel(Channel* ch) const {
    int fd = ch->getFd();
    errif(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) < 0, "epoll delete error");
    ch->setInEpoll(false);
}

std::vector<Channel*> Epoll::poll(int timeout) const {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds < 0, "epoll wait error");
    std::vector<Channel*> active_channels;
    for (int i = 0; i < nfds; ++i) {
        Channel* ch = (Channel*)events[i].data.ptr;
        ch->setReady(events[i].events);
        active_channels.push_back(ch);
    }
    return active_channels;
}


