#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1024 
Epoll::Epoll() {
    epfd = epoll_create(MAX_EVENTS);
    errif(epfd < 0, "epoll create error");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(events) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (epfd >= 0) {
        close(epfd);
        epfd = -1;
    }
    delete[] events;
}

void Epoll::addFd(int fd, uint32_t op) {
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) < 0, "epoll add fd error");
}

std::vector<struct epoll_event> Epoll::poll(int timeout) {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds < 0, "epoll wait error");
    std::vector<struct epoll_event> active_fds;
    for (int i = 0; i < nfds; ++i) {
        active_fds.push_back(events[i]);
    }
    return active_fds;
}
