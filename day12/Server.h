#pragma once
#include <map>
#include <vector>
#include <functional>
#include <mutex>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class Server {
private:
    EventLoop* mainReactor;
    std::vector<EventLoop*> subReactors;
    Acceptor* acceptor;
    ThreadPool* thp;
    std::map<int, Connection*> connections;
    std::mutex connectionsMutex;
    int new_cnt = 0;
    int del_cnt = 0;
public:
    Server(EventLoop*);
    ~Server();
    void newConnection(Socket*);
    void handleReadEvent(int);
    void deleteConnection(int);
};