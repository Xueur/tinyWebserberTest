#pragma once
#include "common.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
class EventLoop;
class TcpConnection;
class Acceptor;
class EventLoopThreadPool;
class TcpServer {
public:
    DISALLOW_COPY_AND_MOVE(TcpServer);
    TcpServer(EventLoop* loop,const char * ip, const int port);
    ~TcpServer();
    void Start();
    void newConnection(int);
    inline void deleteConnection(const std::shared_ptr<TcpConnection> & conn);
    inline void deleteConnectionInLoop(const std::shared_ptr<TcpConnection> & conn);
    void setConnectCallback(std::function < void(const std::shared_ptr<TcpConnection> &)> const &fn);
    void setMessageCallback(std::function < void(const std::shared_ptr<TcpConnection> &)> const &fn);

private:
    int next_connid;
    std::unique_ptr<EventLoop> mainReactor;
    std::unique_ptr<Acceptor> acceptor;
    std::vector<std::unique_ptr<EventLoop>> subReactors;
    std::unordered_map<int, std::shared_ptr<TcpConnection>> connections;
    std::unique_ptr<EventLoopThreadPool> thp;
    std::function<void(const std::shared_ptr<TcpConnection> &)> on_connect_;
    std::function<void(const std::shared_ptr<TcpConnection> &)> on_message_;

};