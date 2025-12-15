#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1000

int main(int argc, char const *argv[])
{
    Socket* serv_sock = new Socket();
    InetAddress* serv_adr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_adr);
    serv_sock->listen();
    serv_sock->setnonblocking();
    Epoll *epoll = new Epoll();
    epoll->addFd(serv_sock->getFd(), EPOLLIN | EPOLLET);
    while (true) {
        std::vector<struct epoll_event> events = epoll->poll(-1);
        int epfds = events.size();
        for (int i = 0; i < epfds; i++) {
            int sockfd = events[i].data.fd;
            if (sockfd == serv_sock->getFd()) { //建立新的连接
                InetAddress clnt_adr;
                int clnt_sock = serv_sock->accept(&clnt_adr);
                errif(clnt_sock < 0, "accept error");
                printf("new client connected: %s:%d\n",
                       inet_ntoa(clnt_adr.addr.sin_addr),
                       ntohs(clnt_adr.addr.sin_port));
                Socket* clnt_socket = new Socket(clnt_sock);
                clnt_socket->setnonblocking();
                epoll->addFd(clnt_socket->getFd(), EPOLLIN | EPOLLET);
            } else if (events[i].events & EPOLLIN) { //处理读事件
                char buf[READ_BUFFER];
                while (true) {
                    ssize_t str_len = read(sockfd, buf, READ_BUFFER - 1);
                    if (str_len > 0) {
                        buf[str_len] = 0; 
                        printf("read %ld bytes from client(fd=%d): %s\n",
                               str_len, sockfd, buf);
                        //echo back
                        write(sockfd, buf, str_len);
                    } else if (str_len < 0 && errno == EINTR) {
                        //被信号中断，继续读取
                        printf("continue reading\n");
                        continue;
                    } else if (str_len < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                        //read完毕
                        printf("finish reading\n");
                        break;
                    } else if (str_len == 0) {
                        //客户端关闭连接
                        printf("client(fd=%d) disconnected\n", sockfd);
                        close(sockfd);//关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
                }
            }
        }
    }
    delete serv_sock;
    delete serv_adr;
    delete epoll;
    return 0;
}
