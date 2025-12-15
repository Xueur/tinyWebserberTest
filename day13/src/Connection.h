#pragma once 
#include <functional>
#include <string>

class Buffer;
class Channel;
class EventLoop;
class Socket;
class Epoll;
class Connection {
public:
    enum State {
        Invalid = 1,
        Handshaking,
        Connected,
        Closed,
        Failed,
    };
    Connection(EventLoop*, Socket*);
    ~Connection();

    void Read();
    void Write();
    void setDeleteConnectionCallback(std::function<void(int)> const & _cb);
    void setOnConnectionCallback(std::function<void(Connection*)> const & _cb);
    State getState();
    void Close();
    void setSendBuffer(const char* str);
    Buffer *getRecvBuffer();
    const char *RecvBuffer();
    Buffer *getSendBuffer();
    const char *sendBuffer();
    void getlineSendBuffer();
    Socket *GetSocket();
    void OnConnection(std::function<void()> _cb);
private:
    EventLoop* loop;
    Channel* ch;
    Socket* sock;
    std::function<void(int)> deleteConnectionCallback;
    State state_{State::Invalid};
    Buffer* recvBuf{nullptr};
    Buffer* sendBuf{nullptr};
    std::function<void(Connection *)> onConnectionCallback;
    void recvNonBlocking();
    void sendNonBlocking();
    void recvBlocking();
    void sendBlocking();
};