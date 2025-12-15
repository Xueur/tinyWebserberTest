#include "TcpConnection.h"
#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "util.h"
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <sys/socket.h>

TcpConnection::TcpConnection(EventLoop* _loop, int _connfd, int _connid)
    : connfd(_connfd), connid(_connid), loop(_loop) {
    if (loop != nullptr) {
        printf("New connection established. ConnID: %d, ConnFD: %d\n", connid, connfd);
        state = State::Connected;
        ch = std::make_unique<Channel>(loop, connfd);
        ch->useET();
        ch->setReadCallback(std::bind(&TcpConnection::handleMessage, this));
        ch->enableReading();
    }
    recvBuf = std::make_unique<Buffer>();
    sendBuf = std::make_unique<Buffer>();
}

TcpConnection::~TcpConnection(){
    ::close(connfd);
}

void TcpConnection::setCloseTcpConnectionCallback(std::function<void(int)> const &fn) { 
    on_close_ = std::move(fn); 
}
void TcpConnection::setMessageCallback(std::function<void(TcpConnection *)> const &fn) { 
    on_message_ = std::move(fn);
}

void TcpConnection::handleMessage(){
    Read();
    if (on_message_)
    {
        on_message_(this);
    }
}

void TcpConnection::handleClose() {
    //std::cout << CurrentThread::tid() << " TcpConnection::HandleClose" << std::endl;
    if (state != State::Closed)
    {
        state = State::Closed;
        if(on_close_){
            on_close_(connfd);
        }
    }
}



EventLoop *TcpConnection::getLoop() const { return loop; }
int TcpConnection::getId() const { return connid; }
int TcpConnection::getFd() const { return connfd; }
TcpConnection::State TcpConnection::getState() const { return state; }
void TcpConnection::setSendBuffer(const char *str) { sendBuf->setBuf(str); }
Buffer *TcpConnection::getRecvBuf() { return recvBuf.get(); }
Buffer *TcpConnection::getSendBuf() { return sendBuf.get(); }

void TcpConnection::Send(const std::string &msg){
    setSendBuffer(msg.c_str());
    Write();
}

void TcpConnection::Send(const char *msg){
    printf("%s\n", msg);
    setSendBuffer(msg);
    Write();
}

void TcpConnection::Send(const char *msg, int len){
    char buf[len + 1];
    memset(buf, 0, sizeof(buf));
    strncpy(buf, msg, len);
    setSendBuffer(buf);
    Write();
}

void TcpConnection::Read()
{
    recvBuf->clear();
    recvNonBlocking();
}

void TcpConnection::Write(){
    sendNonBlocking();
    sendBuf->clear();
}

void TcpConnection::recvNonBlocking() {
    char buf[1024];  // 这个buf大小无所谓
    while (true) {   // 使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(connfd, buf, sizeof(buf));
        if (bytes_read > 0) {
            printf("read %ld bytes from client fd %d\n", bytes_read, connfd);
            recvBuf->append(buf, bytes_read);
        } else if (bytes_read == -1 && errno == EINTR) {  // 程序正常中断、继续读取
            printf("continue reading\n");
            continue;
        } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
            break;
        } else if (bytes_read == 0) {  // EOF，客户端断开连接
            printf("read EOF, client fd %d disconnected\n", connfd);
            state = State::Closed;
            break;
        } else {
            printf("Other error on client fd %d\n", connfd);
            state = State::Closed;
            break;
        }
    }
}

void TcpConnection::sendNonBlocking() {
    char buf[sendBuf->size()];
    memcpy(buf, sendBuf->c_str(), sendBuf->size());
    int data_size = sendBuf->size();
    int data_left = data_size;
    while (data_left > 0) {
        ssize_t bytes_write = write(connfd, buf + data_size - data_left, data_left);
        if (bytes_write == -1 && errno == EINTR) {
          printf("continue writing\n");
          continue;
        }
        if (bytes_write == -1 && errno == EAGAIN) {
          break;
        }
        if (bytes_write == -1) {
          printf("Other error on client fd %d\n", connfd);
          state = State::Closed;
          break;
        }
        data_left -= bytes_write;
    }
}
