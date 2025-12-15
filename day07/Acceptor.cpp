#include "Acceptor.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "InetAddress.h"

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
    newConnectionCallback(serv_sock);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> cb) {
    newConnectionCallback = cb;
}