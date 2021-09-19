#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]) {
    if(argc <= 2) {
        printf("useage : %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    
    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(socket >= 0);

    int ret = bind(sock, (sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (sockaddr*)&client, &client_addrlength);
    if(connfd < 0) {
        printf("errno is: %d\n", errno);
    }
    else {
        send(connfd, "welcome", 8, 0);
        while(true) {
            char buffer[BUF_SIZE];

            memset(buffer, '\0', BUF_SIZE);
            ret = recv(connfd, buffer, BUFSIZ - 1, 0);
            if(ret <= 0) break;
            printf("got %d bytes of normal data '%s'\n", ret, buffer);

            if(buffer == "bye") {
                break;
            }

            // memset(buffer, '\0', BUF_SIZE);
            // ret = recv(connfd, buffer, BUFSIZ - 1, MSG_OOB);
            // printf("got %d bytes of oob data '%s'\n", ret, buffer);

            send(connfd, buffer, ret, 0);
        }
        close(connfd);
    }
    close(sock);
    return 0;
}