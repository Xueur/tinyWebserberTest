#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "util.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Buffer.h"

#define BUFFER_SIZE 1024 

int main(int argc, char const *argv[])
{
    Socket* sock = new Socket();
    InetAddress* serv_adr = new InetAddress("127.0.0.1", 8080);
    sock-> connect(serv_adr);
    int sockfd = sock->getFd();
    Buffer* sendBuf = new Buffer();
    Buffer* recvBuf = new Buffer();
    while (true) {
        printf("Input message to send to server: ");
        fflush(stdout);
        sendBuf->getline();
        ssize_t write_bytes = write(sockfd, sendBuf->c_str(), sendBuf->size());
        if (write_bytes == -1) {
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        int already_read = 0;
        char buf[BUFFER_SIZE];
        while (true) {
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if (read_bytes > 0) {
                recvBuf->append(buf, sizeof(buf));
                already_read += read_bytes;
            } else if (read_bytes == 0) {
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            } 
            if (already_read >= sendBuf->size()) {
                printf("message from server: %s\n", recvBuf->c_str());
                break;
            }
        }
        recvBuf->clear();
    }
    delete sock;
    delete serv_adr;
    delete sendBuf;
    delete recvBuf;
    return 0;
}
