#include "Connection.h"
#include "Channel.h"
#include "Socket.h" 
#include "Buffer.h"
#include "util.h"
#include <unistd.h>
#include <string.h>
#include <iostream>
#define READ_BUFFER 1024

Connection::Connection(EventLoop* _loop, Socket* _sock): loop(_loop), sock(_sock), ch(nullptr), recvBuf(nullptr) {
    ch = new Channel(loop, sock->getFd());
    ch->enableReading();
    ch->useET();
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    ch->setReadCallback(cb);
    recvBuf = new Buffer();
}

Connection::~Connection() {
    if (ch) {
        delete ch;
        ch = nullptr;
    }
    delete sock;
    delete recvBuf;
}

void Connection::echo(int sockfd) {
    char buf[READ_BUFFER];
    while (true) {
        bzero(&buf, sizeof(buf));
        ssize_t str_len = read(sockfd, buf, sizeof(buf));
        // printf("read %ld bytes from client(fd=%d)\n", str_len, sockfd);
        if (str_len > 0) {
            recvBuf->append(buf, str_len);
        } else if (str_len == -1 && errno == EINTR) {
            //被信号中断，继续读取
            printf("continue reading\n");
            continue;
        } else if (str_len == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            //read完毕
            printf("from client(fd=%d): %s\n", sockfd, recvBuf->c_str());
            // errif(write(sock->getFd(), recvBuf->c_str(), recvBuf->size()) < 0, "socket write error");
            send(sockfd);
            recvBuf->clear();
            break;
        } else if (str_len == 0) {
            //客户端关闭连接
            printf("client(fd=%d) disconnected\n", sockfd);
            deleteConnectionCallback(sockfd);
            break;
        } else {
            printf("Connection reset by peer\n");
            deleteConnectionCallback(sockfd);         
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> _cb) {
    deleteConnectionCallback = _cb;
}

void Connection::send(int sockfd){
    char buf[recvBuf->size()];
    strcpy(buf, recvBuf->c_str());
    int  data_size = recvBuf->size(); 
    int data_left = data_size; 
    while (data_left > 0) 
    { 
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left); 
        if (bytes_write == -1 && errno == EAGAIN) { 
            break;
        }
        data_left -= bytes_write; 
    }
}