#include "Connection.h"
#include "Channel.h"
#include "Socket.h" 
#include "Buffer.h"
#include "util.h"
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <assert.h>
#define READ_BUFFER 1024

Connection::Connection(EventLoop* _loop, Socket* _sock): loop(_loop), sock(_sock), ch(nullptr), recvBuf(nullptr) {
    if (loop != nullptr && sock != nullptr) {
        ch = new Channel(loop, sock->getFd());
        ch->enableReading();
        ch->useET();
    }
    recvBuf = new Buffer();
    sendBuf = new Buffer();
    state_ = State::Connected;
}

Connection::~Connection() {
    if (ch) {
        delete ch;
        ch = nullptr;
    }
    delete sock;
    delete sendBuf;
    delete recvBuf;
}
void Connection::Read() {
    assert(state_ == State::Connected);
    recvBuf->clear();
    recvNonBlocking();
}
void Connection::Write() {
    assert(state_ == State::Connected);
    sendNonBlocking();
}

void Connection::recvNonBlocking() {
    int sockfd = sock->getFd();
  char buf[1024];  // 这个buf大小无所谓
  while (true) {   // 使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
    memset(buf, 0, sizeof(buf));
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      recvBuf->append(buf, bytes_read);
    } else if (bytes_read == -1 && errno == EINTR) {  // 程序正常中断、继续读取
      printf("continue reading\n");
      continue;
    } else if (bytes_read == -1 &&
               ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("read EOF, client fd %d disconnected\n", sockfd);
      state_ = State::Closed;
      break;
    } else {
      printf("Other error on client fd %d\n", sockfd);
      state_ = State::Closed;
      break;
    }
  }
}

void Connection::sendNonBlocking() {
    int sockfd = sock->getFd();
  char buf[sendBuf->size()];
  memcpy(buf, sendBuf->c_str(), sendBuf->size());
  int data_size = sendBuf->size();
  int data_left = data_size;
  while (data_left > 0) {
    ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
    if (bytes_write == -1 && errno == EINTR) {
      printf("continue writing\n");
      continue;
    }
    if (bytes_write == -1 && errno == EAGAIN) {
      break;
    }
    if (bytes_write == -1) {
      printf("Other error on client fd %d\n", sockfd);
      state_ = State::Closed;
      break;
    }
    data_left -= bytes_write;
  }
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> const & _cb) {
    deleteConnectionCallback = _cb;
}

void Connection::setOnConnectionCallback(std::function<void(Connection*)> const & _cb) {
    onConnectionCallback = _cb; 
    ch->setReadCallback([this]() {onConnectionCallback(this);});
}

Connection::State Connection::getState() {
    return state_;
}

void Connection::Close() {
    deleteConnectionCallback(sock->getFd());
}

void Connection::setSendBuffer(const char* str) {
    sendBuf->setBuf(str);
}

Buffer* Connection::getRecvBuffer() {
    return recvBuf;
}

const char* Connection::RecvBuffer() {
    return recvBuf->c_str();
}

Buffer* Connection::getSendBuffer() {
    return sendBuf;
}

const char* Connection::sendBuffer() {
    return sendBuf->c_str();
}

void Connection::getlineSendBuffer() {
    sendBuf->getline();
}

Socket* Connection::GetSocket() {
    return sock;
}

