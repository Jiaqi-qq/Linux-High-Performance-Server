#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#define MAXLINE 5
#define OPEN_MAX 1024
#define SA struct sockaddr


int main() {
    int listenfd, connfd, sockfd, i, maxi;
    int nready;
    socklen_t clilen;
    ssize_t n;
    char buf[MAXLINE];
    struct pollfd client[OPEN_MAX];
    struct sockaddr_in servaddr, cliaddr;
    //创建监听套接字
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket() error!");
        exit(0);
    }
    //先要对协议地址进行清零
    bzero(&servaddr,sizeof(servaddr));
    //设置为 IPv4 or IPv6
    servaddr.sin_family = AF_INET;
    //绑定本地端口号
    servaddr.sin_port    = htons(9805);
    //任何一个 IP 地址，让内核自行选择
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //绑定套接口到本地协议地址
    if(bind(listenfd, (SA *) &servaddr,sizeof(servaddr)) < 0) {
        printf("bind() error!");
        exit(0);
    }
    //服务器开始监听
    if(listen(listenfd,5) < 0) {
        printf("listen() error!");
        exit(0);
    }
    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;//关心监听套机字的读事件
    for(i = 1; i < OPEN_MAX; ++i) {
        client[i].fd = -1;
    }
    maxi = 0;
    for(;;) {
        nready = poll(client, maxi + 1, -1);
        if(client[0].revents & POLLRDNORM) {
            clilen = sizeof(cliaddr);
            //accept 的后面两个参数都是值-结果参数，他们的保留的远程连接电脑的信息，如果不管新远程连接电脑的信息，可以将这两个参数设置为 NULL
            connfd = accept(listenfd, (SA *) &cliaddr, &clilen);
            if(connfd < 0) {
                continue;
            }
            for(i = 1; i < OPEN_MAX; ++i) {
                if(client[i].fd < 0)
                    client[i].fd = connfd, client[i].events = 0;
                break;
            }
            if(i == OPEN_MAX) {
                printf("too many clients");
                exit(0);
            }
            client[i].events = POLLRDNORM;
            if(i > maxi) {
                maxi = i;
            }
            if(--nready <=0 )
                continue;
        }
        for(i = 1; i < OPEN_MAX; ++i) {
            if((sockfd = client[i].fd) < 0) {
                continue;
            }
            if(client[i].revents & POLLRDNORM | POLLERR) {
                if((n = read(sockfd, buf, MAXLINE)) < 0) {
                    if(errno == ECONNRESET) {
                        close(sockfd);
                        client[i].fd = -1;
                    }
                    else {
                        printf("read error!\n");
                    }
                }
                else if(n == 0) {
                    close(sockfd);
                    client[i].fd = -1;
                }
                else {
                    write(sockfd, buf,  n);
                }
                if(--nready <= 0)
                    break;
            }
        }
    }
}