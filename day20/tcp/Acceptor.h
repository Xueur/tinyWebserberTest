#pragma once
#include <functional>
#include <memory>
#include "common.h"

class EventLoop;
class Channel;
class Acceptor {
public:
    DISALLOW_COPY_AND_MOVE(Acceptor);
    Acceptor(EventLoop *_loop, const char * ip, const int port);
    ~Acceptor();
    //原Socket类中的操作
    void Create();
    void Bind(const char *ip, const int port);
    void Listen();
    void acceptConnection();
    void setNewConnectionCallback(std::function<void(int)> const& cb);
private:
    EventLoop *loop;
    int listen_fd;
    std::unique_ptr<Channel> accept_channel;
    std::function<void(int)> newConnectionCallback;
};
