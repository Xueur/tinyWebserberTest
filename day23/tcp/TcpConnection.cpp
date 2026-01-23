#include "TcpConnection.h"
#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "HttpContext.h"
#include "util.h"
#include "Logging.h"
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <sys/socket.h>

TcpConnection::TcpConnection(EventLoop* _loop, int _connfd, int _connid)
    : loop(_loop), connfd(_connfd), connid(_connid) {
    if (loop != nullptr) {
        ch = std::make_unique<Channel>(loop, connfd);
        ch->useET();
        ch->setReadCallback(std::bind(&TcpConnection::handleMessage, this)); 
        ch->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    }
    recvBuf = std::make_unique<Buffer>();
    sendBuf = std::make_unique<Buffer>();
    context_ = std::make_unique<HttpContext>();
}

TcpConnection::~TcpConnection(){
    printf("TcpConnection::~TcpConnection() called\n");
}

void TcpConnection::ConnectionEstablished(){
    state = State::Connected;
    ch->Tie(shared_from_this());
    ch->enableReading();
    if (on_connect_){
        on_connect_(shared_from_this());
    }
}

void TcpConnection::ConnectionDestructor(){
    loop->deleteChannel(ch.get());
}

void TcpConnection::setOnConnectCallback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn){
    on_connect_ = std::move(fn);
}
void TcpConnection::setCloseTcpConnectionCallback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn) { 
    on_close_ = std::move(fn); 
}
void TcpConnection::setMessageCallback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn) { 
    on_message_ = std::move(fn);
}

void TcpConnection::handleMessage(){
    Read();
    if (on_message_)
    {
        on_message_(shared_from_this());
    }
}

void TcpConnection::handleWrite(){

    LOG_INFO << "TcpConnection::HandlWrite";
    Write();
}

void TcpConnection::handleClose() {
    //std::cout << CurrentThread::tid() << " TcpConnection::HandleClose" << std::endl;
    if (state != State::Closed)
    {
        state = State::Closed;
        if(on_close_){
            on_close_(shared_from_this());
        }
    }
}

EventLoop *TcpConnection::getLoop() const { return loop; }
int TcpConnection::getId() const { return connid; }
int TcpConnection::getFd() const { return connfd; }
TcpConnection::State TcpConnection::getState() const { return state; }
void TcpConnection::setSendBuffer(const char *str) { sendBuf->append(str); }
Buffer *TcpConnection::getRecvBuf() { return recvBuf.get(); }
Buffer *TcpConnection::getSendBuf() { return sendBuf.get(); }

void TcpConnection::Send(const std::string &msg){
    Send(msg.data(), static_cast<int>(msg.size()));
}

void TcpConnection::Send(const char *msg){
    Send(msg, static_cast<int>(strlen(msg)));
} 


void TcpConnection::Send(const char *msg, int len){

    int remaining = len;
    int send_size = 0;

    // 如果此时sendBuf中没有数据，则可以先尝试发送数据， 
    if (sendBuf->readablebytes() == 0){
        send_size = static_cast<int>(write(connfd, msg, len));

        if(send_size >= 0){
            // 说明发送了部分数据
            remaining -= send_size;
        }else if((send_size == -1) && 
                    ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
            send_size = 0;// 说明实际上没有发送数据
        }
        else{
            LOG_ERROR << "TcpConnection::Send - TcpConnection Send ERROR";
            return;
        }
    }
    // 将剩余的数据加入到send_buf中，等待后续发送。
    assert(remaining <= len);
    if(remaining > 0){
        sendBuf->append(msg + send_size, remaining);

        // 到达这一步时
        // 1. 还没有监听写事件，在此时进行了监听
        // 2. 监听了写事件，并且已经触发了，此时再次监听，强制触发一次，如果强制触发失败，仍然可以等待后续TCP缓冲区可写。
        ch->enableWriting();
    }
}

void TcpConnection::Read()
{
    recvBuf->RetrieveAll();
    recvNonBlocking();
}

void TcpConnection::Write(){
    sendNonBlocking();
    sendBuf->RetrieveAll();
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
            handleClose();
            break;
        } else {
            handleClose();
            break;
        }
    }
}

void TcpConnection::sendNonBlocking() {
    int remaining = sendBuf->readablebytes();
    int send_size = static_cast<int>(write(connfd, sendBuf->Peek(), remaining));
    if((send_size == -1) && 
                ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
        // 说明此时TCP缓冲区是满的，没有办法写入，什么都不做 
        // 主要是防止，在Send时write后监听EPOLLOUT，但是TCP缓冲区还是满的，
        send_size = 0; // 在后续`Retrieve`处使用
    }
    else if (send_size == -1){
        LOG_ERROR << "TcpConnection::Send - TcpConnection Send ERROR";
    }

    remaining -= send_size;
    sendBuf->Retrieve(send_size);
}

HttpContext *TcpConnection::context() const { return context_.get(); }

TimeStamp TcpConnection::getLastActiveTime() const {
    return last_active_time_;
}

void TcpConnection::updateLastActiveTime(TimeStamp time) {
    last_active_time_ = time;
}