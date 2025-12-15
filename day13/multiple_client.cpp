#include <iostream>
#include <unistd.h>
#include <string.h>
#include <functional>
#include "src/util.h"
#include "src/Buffer.h"
#include "src/Socket.h"
#include "src/ThreadPool.h"
#include "src/InetAddress.h"
#include <memory>

using namespace std;

void set_timeout(int sockfd, int seconds){
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
}

void oneClient(int msgs, int wait){
    auto sock = std::make_unique<Socket>();
    auto addr = std::make_unique<InetAddress>("127.0.0.1", 8080);
    // sock->setnonblocking(); 
    sock->connect(addr.get());

    int sockfd = sock->getFd();
    set_timeout(sockfd, 5);

    auto sendBuffer = std::make_unique<Buffer>();
    auto readBuffer = std::make_unique<Buffer>();
    sleep(wait);
    int count = 0;
    while(count < msgs){
        sendBuffer->setBuf("I'm client!");
        ssize_t write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size());
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        int already_read = 0;
        char buf[1024];    //这个buf大小无所谓
        while(true){
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if(read_bytes > 0){
                readBuffer->append(buf, read_bytes);
                already_read += read_bytes;
            } else if(read_bytes == 0){         
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            } else {
                errif(true, "socket read error");
                exit(EXIT_FAILURE);
            }
            if(already_read >= sendBuffer->size()){
                printf("count: %d, message from server: %s\n", count++, readBuffer->c_str());
                break;
            } 
        }
        readBuffer->clear();
    }
}

int main(int argc, char *argv[]) {
    int threads = 100;
    int msgs = 100;
    int wait = 0;
    int o;
    const char *optstring = "t:m:w:";
    while ((o = getopt(argc, argv, optstring)) != -1) {
        switch (o) {
            case 't':
                threads = stoi(optarg);
                break;
            case 'm':
                msgs = stoi(optarg);
                break;
            case 'w':
                wait = stoi(optarg);
                break;
            case '?':
                printf("error optopt: %c\n", optopt);
                printf("error opterr: %d\n", opterr);
                break;
        }
    }

    auto pool = std::make_unique<ThreadPool>(6);
    std::function<void()> func = std::bind(oneClient, msgs, wait);
    for(int i = 0; i < threads; ++i){
        pool->add(func);
    }
    return 0;
}