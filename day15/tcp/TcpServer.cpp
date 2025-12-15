#include "TcpServer.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include "common.h"
#include "CurrentThread.h"
#include <unistd.h>
#include <iostream>
#include <memory>
#include <assert.h>


TcpServer::TcpServer(const char * ip, const int port): next_connid(1){
    mainReactor= std::make_unique<EventLoop>();
    acceptor = std::make_unique<Acceptor>(mainReactor.get(), ip, port);
    std::function<void(int)> cb = std::bind(&TcpServer::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);

    unsigned int size = 6;
    thp = std::make_unique<ThreadPool>(size);
    for (size_t i = 0; i < size; ++i){
        std::unique_ptr<EventLoop> sub_reactor = std::make_unique<EventLoop>();
        subReactors.push_back(std::move(sub_reactor));
    }
}

TcpServer::~TcpServer(){

};

void TcpServer::Start(){
    for (size_t i = 0; i < subReactors.size(); ++i){
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i].get());
        thp->add(std::move(sub_loop));
    }
    mainReactor->loop();
}

void TcpServer::newConnection(int fd){
    assert(fd != -1);
    uint64_t random = fd % subReactors.size();
    std::shared_ptr<TcpConnection> conn = std::make_shared<TcpConnection>(subReactors[random].get(), fd, next_connid);
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

void TcpServer::setMessageCallback(std::function < void(const std::shared_ptr<TcpConnection> &)> const &fn) {on_message_ = std::move(fn); };
void TcpServer::setConnectCallback(std::function < void(const std::shared_ptr<TcpConnection> &)> const &fn) { on_connect_ = std::move(fn); };