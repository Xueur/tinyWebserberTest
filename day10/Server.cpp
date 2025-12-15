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
    Connection* conn = new Connection(loop, clnt_sock);
    std::function<void(Socket*)> cb = std::bind(&Server:: deleteConnection, this, std::placeholders::_1);
    conn->setDeleteConnectionCallback(cb);
    connections[clnt_sock->getFd()] = conn;
}

void Server::deleteConnection(Socket* sock) {
    Connection* conn = connections[sock->getFd()];
    connections.erase(sock->getFd());
    delete conn;
}
