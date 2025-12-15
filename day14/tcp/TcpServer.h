#pragma once
#include "common.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
class EventLoop;
class TcpConnection;
class Acceptor;
class ThreadPool;
class TcpServer {
public:
    DISALLOW_COPY_AND_MOVE(TcpServer);
    TcpServer(const char *ip, const int port);
    ~TcpServer();
    void Start();
    void newConnection(int);
    void deleteConnection(int);

    void setCloseCallback(std::function < void(TcpConnection *)> const &fn);
    void setMessageCallback(std::function < void(TcpConnection *)> const &fn);

private:
    int next_connid;
    std::unique_ptr<EventLoop> mainReactor;
    std::unique_ptr<Acceptor> acceptor;
    std::vector<std::unique_ptr<EventLoop>> subReactors;
    std::unordered_map<int, TcpConnection *> connections;
    std::unique_ptr<ThreadPool> thp;
    std::function<void(TcpConnection *)> on_connect_;
    std::function<void(TcpConnection *)> on_message_;
};