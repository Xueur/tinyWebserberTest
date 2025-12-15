#include "Server.h"
#include "EventLoop.h"

int main(int argc, char const *argv[])
{
    EventLoop* loop = new EventLoop();
    Server server(loop);
    loop->loop();
    delete loop;
    return 0;
}