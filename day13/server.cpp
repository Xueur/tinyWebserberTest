#include "src/Server.h"
#include "src/Buffer.h"
#include "src/Connection.h"
#include "src/EventLoop.h"
#include "src/Socket.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    EventLoop* loop = new EventLoop();
    Server* server = new Server(loop);
    server->onConnection([](Connection* conn){
        conn->Read();
        if (conn->getState() == Connection::State::Closed) {
            std::cout << "Connection closed by client." << std::endl;
            conn->Close();
        } else {
            std::cout << "Received message: " << conn->RecvBuffer() << std::endl;
            conn->setSendBuffer(conn->RecvBuffer());
            conn->Write();
        }
    });
    loop->loop();
    delete loop;
    delete server;
    return 0;
}