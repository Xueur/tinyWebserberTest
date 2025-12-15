#pragma once 
#include <functional>
#include <string>

class Buffer;
class Channel;
class EventLoop;
class Socket;
class Epoll;
class Connection {
private:
    EventLoop* loop;
    Channel* ch;
    Socket* sock;
    std::function<void(int)> deleteConnectionCallback;
    Buffer* recvBuf;
public:
    Connection(EventLoop*, Socket*);
    ~Connection();

    void echo(int);
    void setDeleteConnectionCallback(std::function<void(int)>);
    void send(int);
};