#include "Server.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "InetAddress.h"
#include "util.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <functional>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <mutex>

#define READ_BUFFER 1000

Server::Server(EventLoop* _loop):mainReactor(_loop), acceptor(nullptr) {
    acceptor = new Acceptor(mainReactor);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);

    int size = 6;
    thp = new ThreadPool(size);
    for (int i = 0; i < size; ++i) {
        subReactors.push_back(new EventLoop());
    }
    for (int i = 0; i < size; ++i) {
        std::function<void()> func = std::bind(&EventLoop::loop, subReactors[i]);
        thp->add(func);
    }
}

Server::~Server() {
    delete acceptor;
    delete thp;
}

void Server::newConnection(Socket *sock){
    if(sock->getFd() != -1){
        int random = sock->getFd() % subReactors.size();
        Connection *conn = new Connection(subReactors[random], sock);
        std::function<void(int)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        conn->setDeleteConnectionCallback(cb);
        {
            std::unique_lock<std::mutex> lock(connectionsMutex);
            connections[sock->getFd()] = conn;
        }
    }
}

void Server::deleteConnection(int sockfd){
    if(sockfd != -1){
        Connection *conn = nullptr;
        {
            std::unique_lock<std::mutex> lock(connectionsMutex);
            auto it = connections.find(sockfd);
            if(it != connections.end()){
                conn = it->second;
                connections.erase(it);
            }
        }
        if (conn) {
            close(sockfd);
            delete conn;
        }
    }
}