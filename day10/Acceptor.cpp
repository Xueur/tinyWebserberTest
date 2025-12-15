#include "Acceptor.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "InetAddress.h"
#include "util.h"
#include <stdio.h>

Acceptor::Acceptor(EventLoop* _loop):loop(_loop) {
    serv_sock = new Socket();
    serv_adr = new InetAddress("127.0.0.1", 8080);
    serv_sock->bind(serv_adr);
    serv_sock->listen();
    accept_channel = new Channel(loop, serv_sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    accept_channel->setCallBack(cb);
    accept_channel->enableReading();
}

Acceptor::~Acceptor() {
    delete serv_sock;
    delete serv_adr;
    delete accept_channel;
}

void Acceptor::acceptConnection() {
    InetAddress clnt_adr;
    int clnt_sock = serv_sock->accept(&clnt_adr);
    errif(clnt_sock < 0, "accept error");
    printf("new client connected: %s:%d\n",
            inet_ntoa(clnt_adr.addr.sin_addr),
            ntohs(clnt_adr.addr.sin_port));
    Socket* clnt_socket = new Socket(clnt_sock);
    clnt_socket->setnonblocking();
    newConnectionCallback(clnt_socket);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> cb) {
    newConnectionCallback = cb;
}