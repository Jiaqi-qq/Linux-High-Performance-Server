#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
int main(int argc, char* argv[]) {
    if (argc <= 2) {
        printf("useage : %s ip_address port_number\n", basename(argv[0]));
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

    assert(socket >= 0);
    if (connect(sockfd, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("connection failed\n");
    } else {
        //    int flag = 1; // close Nagle
        //    int ret = setsockopt (sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
        //    assert(ret != -1);

        char str[1024]{'\0'};
        while (1) {
            printf("i send:");
            memset(str, '\0', 1024);
            cin.getline(str, sizeof(str));
            if (str[0] == '.') {
                send(sockfd, str, strlen(str), MSG_OOB);
            } else {
                send(sockfd, str, strlen(str), 0);
            }

            memset(str, '\0', 1024);
            read(sockfd, str, sizeof(str));
            printf("Server say: [%s]\n", str);
        }
    }
    close(sockfd);
    return 0;
}