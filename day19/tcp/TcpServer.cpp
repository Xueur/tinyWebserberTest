#include "TcpServer.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include "common.h"
#include "CurrentThread.h"
#include <unistd.h>
#include <iostream>
#include <memory>
#include <assert.h>


TcpServer::TcpServer(EventLoop* loop ,const char * ip, const int port):  next_connid(1),mainReactor(loop) {
    acceptor = std::make_unique<Acceptor>(mainReactor.get(), ip, port);
    std::function<void(int)> cb = std::bind(&TcpServer::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);
    thp = std::make_unique<EventLoopThreadPool>(mainReactor.get());
    thp->setThreadPoolSize(6);
}

TcpServer::~TcpServer(){

};

void TcpServer::Start(){
    thp->Start();
    mainReactor->loop();
}

void TcpServer::newConnection(int fd){
    assert(fd != -1);
    EventLoop *sub_reactor = thp->getNextLoop();
    std::shared_ptr<TcpConnection> conn = std::make_shared<TcpConnection>(sub_reactor,  fd, next_connid);
    std::function<void(const std::shared_ptr<TcpConnection> &)> cb = std::bind(&TcpServer::deleteConnection, this, std::placeholders::_1);
    conn->setCloseTcpConnectionCallback(cb);
    conn->setMessageCallback(on_message_);
    conn->setOnConnectCallback(on_connect_);
    connections[fd] = conn;
    // 分配id
    ++next_connid;
    if(next_connid == 1000){
        next_connid = 1;
    }
    conn->ConnectionEstablished();
}

inline void TcpServer::deleteConnection(const std::shared_ptr<TcpConnection> & conn){
    std::cout <<  CurrentThread::tid() << " TcpServer::HandleClose"  << std::endl;
    mainReactor->RunOneFunc(std::bind(&TcpServer::deleteConnectionInLoop, this, conn));
}

inline void TcpServer::deleteConnectionInLoop(const std::shared_ptr<TcpConnection> & conn){
    std::cout << CurrentThread::tid()  << " TcpServer::HandleCloseInLoop - Remove connection id: " <<  conn->getId() << " and fd: " << conn->getId() << std::endl;
    auto it = connections.find(conn->getFd());
    assert(it != connections.end());
    connections.erase(connections.find(conn->getFd()));

    EventLoop *loop = conn->getLoop();
    loop->QueueOneFunc(std::bind(&TcpConnection::ConnectionDestructor, conn));
}

void TcpServer::SetThreadNums(int thread_nums) { thp->setThreadPoolSize(thread_nums); }

void TcpServer::setMessageCallback(std::function < void(const std::shared_ptr<TcpConnection> &)> const &fn) {on_message_ = std::move(fn); };
void TcpServer::setConnectCallback(std::function < void(const std::shared_ptr<TcpConnection> &)> const &fn) { on_connect_ = std::move(fn); };