#pragma once
#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor
{
private:
    EventLoop *loop;
    Socket *serv_sock;
    InetAddress *serv_adr;
    Channel *accept_channel;
    std::function<void(Socket*)> newConnectionCallback;
public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void acceptConnection();
    void setNewConnectionCallback(std::function<void(Socket*)>);
};
