#include "Connection.h"
#include "Channel.h"
#include "Socket.h" 
#include "Buffer.h"
#include "util.h"
#include <unistd.h>
#include <string.h>
#include <iostream>
#define READ_BUFFER 1024

Connection::Connection(EventLoop* _loop, Socket* _sock): loop(_loop), sock(_sock), ch(nullptr), inBuf(new std::string()), recvBuf(nullptr) {
    ch = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this);
    ch->setCallBack(cb);
    ch->enableReading();
    recvBuf = new Buffer();
}

Connection::~Connection() {
    if (ch) {
        delete ch;
        ch = nullptr;
    }
    delete sock;
}

void Connection::echo() {
    char buf[READ_BUFFER];
    while (true) {
        bzero(buf, READ_BUFFER);
        ssize_t str_len = read(sock->getFd(), buf, READ_BUFFER-1);
        if (str_len > 0) {
            recvBuf->append(buf, sizeof(buf));
        } else if (str_len < 0 && errno == EINTR) {
            //被信号中断，继续读取
            printf("continue reading\n");
            continue;
        } else if (str_len < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            //read完毕
            printf("finish reading once\n");
            printf("read %ld bytes from client(fd=%d): %s\n",
                    recvBuf->size(), sock->getFd(), recvBuf->c_str());
            errif(write(sock->getFd(), recvBuf->c_str(), recvBuf->size()) < 0, "socket write error");
            recvBuf->clear();
            break;
        } else if (str_len == 0) {
            //客户端关闭连接
            printf("client(fd=%d) disconnected\n", sock->getFd());
            deleteConnectionCallback(sock);
            close(sock->getFd());//关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket*)> cb) {
    deleteConnectionCallback = cb;
}
