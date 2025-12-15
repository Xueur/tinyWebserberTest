#pragma once 
#include <functional>
#include <string>
#include <memory>
#include "common.h"

class Buffer;
class Channel;
class EventLoop;
class TcpConnection {
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

    void setCloseTcpConnectionCallback(std::function<void(int)> const & _cb);
    void setMessageCallback(std::function<void(TcpConnection*)> const & _cb);
    
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
    std::function<void(int)> on_close_;
    std::function<void(TcpConnection *)> on_message_;
    void recvNonBlocking();
    void sendNonBlocking();
    void recvBlocking();
    void sendBlocking();
};