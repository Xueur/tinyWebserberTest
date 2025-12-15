#include "Server.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "InetAddress.h"
#include "util.h"
#include "Acceptor.h"
#include "Connection.h"
#include <functional>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define READ_BUFFER 1000

Server::Server(EventLoop* _loop):loop(_loop), acceptor(nullptr) {
    acceptor = new Acceptor(loop);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);
}

Server::~Server() {
    delete acceptor;
}

void Server::newConnection(Socket* clnt_sock) {
    if (clnt_sock->getFd() != -1) {
        Connection* conn = new Connection(loop, clnt_sock);
        std::function<void(Socket*)> cb = std::bind(&Server:: deleteConnection, this, std::placeholders::_1);
        conn->setDeleteConnectionCallback(cb);
        connections[clnt_sock->getFd()] = conn;
    }
}

void Server::deleteConnection(Socket* sock) {
    int sockfd = sock->getFd();
    if (sockfd != -1) {
        auto it = connections.find(sockfd);
        if (it != connections.end()) {
            Connection* conn = it->second;
            connections.erase(it);
            close(sockfd);
        }
    }
}
