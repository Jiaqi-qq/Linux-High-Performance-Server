#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <fcntl.h>

using namespace std;

int main(int argc, char* argv[]) {
    if(argc <= 2) {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
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
    assert(sock >= 0);

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
        // close(STDOUT_FILENO);
        dup2(connfd, STDOUT_FILENO);
        printf("finished\n");
 
        sleep(2);
        printf("finished2\n");
        close(connfd);
    }
    close(sock);

    return 0;
}