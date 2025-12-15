#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "util.h"
#include "InetAddress.h"

#define BUFFER_SIZE 1024 

int main(int argc, char const *argv[])
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd < 0, "socket error");
    InetAddress* serv_adr = new InetAddress("127.0.0.1", 8888);
    errif(connect(sockfd, (struct sockaddr*)&serv_adr->addr, serv_adr->addr_len) < 0, "connect error");
    while (true) {
        char buf[BUFFER_SIZE];
        printf("Input message to send to server: ");
        fflush(stdout);
        bzero(buf, BUFFER_SIZE);
        scanf("%s", buf);
        errif(write(sockfd, buf, strlen(buf))< 0, "write error");
        bzero(buf, BUFFER_SIZE);
        ssize_t str_len = read(sockfd, buf, BUFFER_SIZE - 1);
        errif(str_len < 0, "read error");
        if (str_len == 0) {
            printf("server disconnected\n");
            break;
        } else {
            printf("Message from server: %s\n", buf);
        }
    }
    delete serv_adr;
    close(sockfd);
    return 0;
}
