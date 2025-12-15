#include "TcpServer.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include "common.h"
#include <unistd.h>
#include <iostream>
// #include <memory>
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

    TcpConnection *conn = new TcpConnection(subReactors[random].get(), fd, next_connid);
    std::function<void(int)> cb = std::bind(&TcpServer::deleteConnection, this, std::placeholders::_1);

    conn->setCloseTcpConnectionCallback(cb);
    conn->setMessageCallback(on_message_);

    connections[fd] = conn;
    // 分配id
    ++next_connid;
    if(next_connid == 1000){
        next_connid = 1;
    }
}

void TcpServer::deleteConnection(int fd){
    auto it =  connections.find(fd);
    assert(it != connections.end());
    TcpConnection * conn = connections[fd];
    connections.erase(fd);
    // delete conn;
    // 没有析构，所以在这里进行了 close以先关闭连接
    close(fd);
    conn = nullptr;
}

void TcpServer::setCloseCallback(std::function<void(TcpConnection *)> const &fn) { on_connect_ = std::move(fn); };
void TcpServer::setMessageCallback(std::function<void(TcpConnection *)> const &fn) { on_message_ = std::move(fn); };