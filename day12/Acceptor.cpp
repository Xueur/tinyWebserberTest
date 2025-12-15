#include "Acceptor.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "InetAddress.h"
#include "util.h"
#include <stdio.h>
#include <functional>
#include <unistd.h>

Acceptor::Acceptor(EventLoop* _loop):loop(_loop) {
    serv_sock = new Socket();
    serv_adr = new InetAddress("127.0.0.1", 8080);
    serv_sock->bind(serv_adr);
    serv_sock->listen();
    accept_channel = new Channel(loop, serv_sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    accept_channel->setReadCallback(cb);
    accept_channel->enableReading();
}

Acceptor::~Acceptor() {
    close(serv_sock->getFd());
    delete serv_sock;
    delete serv_adr;
    delete accept_channel;
}

void Acceptor::acceptConnection() {
    InetAddress *clnt_addr = new InetAddress();      
    Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));      
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->getAddr().sin_addr), ntohs(clnt_addr->getAddr().sin_port));
    clnt_sock->setnonblocking();
    newConnectionCallback(clnt_sock);
    delete clnt_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> cb) {
    newConnectionCallback = cb;
}