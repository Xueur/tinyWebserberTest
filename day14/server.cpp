#include "tcp/TcpServer.h"
#include "tcp/Buffer.h"
#include "tcp/TcpConnection.h"
#include <memory>

int main(int argc, char const *argv[])
{
    std::unique_ptr<TcpServer> server = std::make_unique<TcpServer>("127.0.0.1", 8080);
    server->setMessageCallback([](TcpConnection* conn){
        printf("Received message from client fd %d: %s\n", conn->getFd(), conn->getRecvBuf()->c_str());
        conn->Send(conn->getRecvBuf()->c_str());
    });
    server->Start();
    return 0;
}