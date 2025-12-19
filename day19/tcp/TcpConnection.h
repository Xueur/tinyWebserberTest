#pragma once 
#include <functional>
#include <string>
#include <memory>
#include "common.h"

class Buffer;
class Channel;
class EventLoop;
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    enum State {
        Invalid = 1,
        Handshaking,
        Connected,
        Closed,
        Failed,
    };
    DISALLOW_COPY_AND_MOVE(TcpConnection);
    TcpConnection(EventLoop* _loop, int _connfd, int _connid);
    ~TcpConnection();
    void ConnectionEstablished();
    void ConnectionDestructor();

    void setOnConnectCallback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn);
    void setCloseTcpConnectionCallback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn);
    void setMessageCallback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn);
    
    void setSendBuffer(const char* str);
    Buffer* getRecvBuf();
    Buffer* getSendBuf();

    void Read();
    void Write();
    void Send(const std::string &msg); // 输出信息
    void Send(const char *msg, int len); // 输出信息
    void Send(const char *msg);

    void handleMessage(); 
    void handleClose();
    HttpContext *context() const; 

    State getState() const;
    EventLoop *getLoop() const;
    int getFd() const;
    int getId() const;

private:
    EventLoop* loop;
    int connfd;
    int connid;
    State state;
    std::unique_ptr<Channel> ch;
    std::unique_ptr<Buffer> recvBuf;
    std::unique_ptr<Buffer> sendBuf;
    std::function<void(std::shared_ptr<TcpConnection>)> on_close_;
    std::function<void(std::shared_ptr<TcpConnection>)> on_message_;
    std::function<void(std::shared_ptr<TcpConnection>)> on_connect_;
    void recvNonBlocking();
    void sendNonBlocking();
    void recvBlocking();
    void sendBlocking();
    std::unique_ptr<HttpContext> context_;
};