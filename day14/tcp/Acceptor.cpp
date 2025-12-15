#include "Acceptor.h"
#include "Channel.h"
#include "EventLoop.h"
#include "util.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

Acceptor::Acceptor(EventLoop* _loop, const char * ip, const int port ): loop(_loop),listen_fd(-1) {
    Create();
    Bind(ip, port);
    Listen();
    accept_channel = std::make_unique<Channel>(loop, listen_fd);
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    accept_channel->setReadCallback(cb);
    accept_channel->enableReading();
}

Acceptor::~Acceptor() {
    loop->deleteChannel(accept_channel.get());
}

void Acceptor::Create() {
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(listen_fd == -1, "socket create error");
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

void Acceptor::Bind(const char *ip, const int port) {
    struct sockaddr_in serv_adr;
    bzero(&serv_adr, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(ip);
    serv_adr.sin_port = htons(port);
    errif(::bind(listen_fd, (sockaddr*)&serv_adr, sizeof(serv_adr)) == -1, "socket bind error");
}

void Acceptor::Listen() {
    errif(::listen(listen_fd, SOMAXCONN) == -1, "socket listen error");
}

void Acceptor::acceptConnection() {
    struct sockaddr_in clnt_adr;
    bzero(&clnt_adr, sizeof(clnt_adr));
    socklen_t clnt_adr_len = sizeof(clnt_adr);
    int clnt_sockfd = ::accept4(listen_fd, (sockaddr*)&clnt_adr, &clnt_adr_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    errif(clnt_sockfd == -1, "socket accept error");
    if (newConnectionCallback) {
        newConnectionCallback(clnt_sockfd);
    }
}

void Acceptor::setNewConnectionCallback(std::function<void(int)> const& cb) {
    newConnectionCallback = std::move(cb);
}