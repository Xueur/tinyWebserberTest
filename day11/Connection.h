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
    std::function<void(Socket*)> deleteConnectionCallback;
    std::string* inBuf;
    Buffer* recvBuf;
public:
    Connection(EventLoop*, Socket*);
    ~Connection();

    void echo();
    void setDeleteConnectionCallback(std::function<void(Socket*)>);
    void send(int);
};