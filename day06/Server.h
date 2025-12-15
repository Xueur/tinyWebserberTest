#pragma once

class EventLoop;
class Socket;
class Server {
private:
    EventLoop* loop;
public:
    Server(EventLoop*);
    ~Server();
    void newConnection(Socket*);
    void handleReadEvent(int);
};