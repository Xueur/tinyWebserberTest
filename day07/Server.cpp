#include "Server.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "InetAddress.h"
#include "util.h"
#include "Acceptor.h"
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

void Server::newConnection(Socket* serv_sock) {
    InetAddress clnt_adr;
    int clnt_sock = serv_sock->accept(&clnt_adr);
    errif(clnt_sock < 0, "accept error");
    printf("new client connected: %s:%d\n",
            inet_ntoa(clnt_adr.addr.sin_addr),
            ntohs(clnt_adr.addr.sin_port));
    Socket* clnt_socket = new Socket(clnt_sock);
    clnt_socket->setnonblocking();
    Channel* clnt_channel = new Channel(loop, clnt_sock);
    std::function<void()> cb = std::bind(&Server:: handleReadEvent, this, clnt_sock);
    clnt_channel->setCallBack(cb);
    clnt_channel->enableReading();
}

void Server::handleReadEvent(int clnt_sock) {
    char buf[READ_BUFFER];
    while (true) {
        bzero(buf, READ_BUFFER);
        ssize_t str_len = read(clnt_sock, buf, READ_BUFFER-1);
        if (str_len > 0) {
            buf[str_len] = 0; 
            printf("read %ld bytes from client(fd=%d): %s\n",
                    str_len, clnt_sock, buf);
            //echo back
            write(clnt_sock, buf, str_len);
        } else if (str_len < 0 && errno == EINTR) {
            //被信号中断，继续读取
            printf("continue reading\n");
            continue;
        } else if (str_len < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            //read完毕
            printf("finish reading\n");
            break;
        } else if (str_len == 0) {
            //客户端关闭连接
            printf("client(fd=%d) disconnected\n", clnt_sock);
            close(clnt_sock);//关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}
