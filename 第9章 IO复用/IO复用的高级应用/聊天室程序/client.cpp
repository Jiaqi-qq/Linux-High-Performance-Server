#define _GNU_SOURCE 1
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <poll.h>
#include <fcntl.h>

#define BUFFER_SIZE 64

int main(int argc, char* argv[]) {
    if(argc <= 2) {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &server_address.sin_addr);
    server_address.sin_port = htons(port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

/*  指定client port
    sockaddr_in my_address;
    bzero(&my_address, sizeof(my_address));
    my_address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &my_address.sin_addr);
    my_address.sin_port = htons(33333);
    bind(sockfd, (sockaddr*)&my_address, sizeof(my_address));
*/
    if(connect(sockfd, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("connect failed\n");
        close(sockfd);
        return 1;
    }

    pollfd fds[2];
    /* 注册文件描述符0(标准输入)和文件描述符sockfd上的可读事件 */
    fds[0].fd = 0, fds[0].events = POLLIN, fds[0].revents = 0;
    fds[1].fd = sockfd, fds[1].events = POLLIN | POLLRDHUP, fds[1].revents = 0;

    char read_buf[BUFFER_SIZE];
    int pipefd[2];
    int ret = pipe(pipefd);
    assert(ret != -1);

    while(true) {
        ret = poll(fds, 2, -1);
        if(ret < 0) {
            printf("poll failure\n");
            break;
        }

        if(fds[1].revents & POLLRDHUP) {
            printf("server close the connection\n");
            break;
        }
        else if(fds[1].revents & POLLIN) {
            memset(read_buf, '\0', sizeof(read_buf));
            recv(fds[1].fd, read_buf, BUFFER_SIZE - 1, 0);
            printf("%s\n", read_buf);
        }

        if(fds[0].revents & POLLIN) {
            /* 使用splice将用户输入的数据直接写到sockfd上（零拷贝） */
            ret = splice(0, nullptr, pipefd[1], nullptr, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
            ret = splice(pipefd[0], nullptr, sockfd, nullptr, 32768, SPLICE_F_MOVE | SPLICE_F_MOVE);
        }
    }
    close(sockfd);
    return 0;
}