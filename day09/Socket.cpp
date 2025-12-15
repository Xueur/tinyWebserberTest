#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

Socket::Socket(): sockfd(-1) {
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    errif(sockfd < 0, "sockrt create error");
}

Socket::Socket(int fd): sockfd(fd) {
    errif(sockfd < 0, "sockrt create error");
}

Socket::~Socket() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
    }
}

void Socket::bind(InetAddress* adr) {
    errif(::bind(sockfd, (struct sockaddr*)&adr->addr, adr->addr_len) < 0, "socket bind error");
}

void Socket::listen() {
    errif(::listen(sockfd, SOMAXCONN) < 0, "socket listen error");
}

void Socket::setnonblocking() {
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* adr) {
    int clnt_fd = ::accept(sockfd, (struct sockaddr*)&adr->addr, &adr->addr_len);
    errif(clnt_fd < 0, "socket accept error");
    return clnt_fd;
}

void Socket::connect(InetAddress* adr) {
    errif(::connect(sockfd, (struct sockaddr*)&adr->addr, adr->addr_len), 0);
}

int Socket::getFd() {
    return sockfd;
}