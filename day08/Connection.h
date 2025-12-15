#pragma once 
#include <functional>

class Channel;
class EventLoop;
class Socket;
class Epoll;
class Connection {
private:
    EventLoop* loop;
    Channel* ch;
    Socket* sock;
    std::function<void(Socket*)> deleteConnectionCallback;
public:
    Connection(EventLoop*, Socket*);
    ~Connection();

    void echo();
    void setDeleteConnectionCallback(std::function<void(Socket*)>);
};