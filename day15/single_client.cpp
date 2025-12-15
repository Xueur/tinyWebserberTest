#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "tcp/util.h"
#include "tcp/Buffer.h"

#define BUFFER_SIZE 1024 

int main(int argc, char const *argv[])
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");
    struct sockaddr_in* serv_adr = new sockaddr_in;
    bzero(serv_adr, sizeof(sockaddr_in));
    serv_adr->sin_family = AF_INET;
    serv_adr->sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr->sin_port = htons(8080);
    int ret = connect(sockfd, (struct sockaddr*)serv_adr, sizeof(sockaddr_in));
    errif(ret == -1, "socket connect error");
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
    close(sockfd);
    delete serv_adr;
    delete sendBuf;
    delete recvBuf;
    return 0;
}
