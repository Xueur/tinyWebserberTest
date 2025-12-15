#pragma once

class EventLoop;
class Socket;
class Acceptor;
class Server {
private:
    EventLoop* loop;
    Acceptor* acceptor;
public:
    Server(EventLoop*);
    ~Server();
    void newConnection(Socket*);
    void handleReadEvent(int);
};