#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <arpa/inet.h>
using namespace std;


/* 在获取daytime时，主机需要开启daytime服务。不然访问会被拒绝。
 * 开启daytime服务：https://blog.csdn.net/LinearF/article/details/80497885 
 */
int main(int argc, char *argv[]) {
    assert(argc == 2);
    char *host = argv[1];
    /* 获取目标主机地质信息 */
    hostent* hostinfo = gethostbyname(host);
    assert(hostinfo);
    /* 获取daytime服务信息 */
    servent* servinfo = getservbyname("daytime", "tcp");
    assert(servinfo);
    printf("daytime port is %d\n", ntohs(servinfo->s_port));

    for(int i=0; hostinfo->h_addr_list[i]; i++){
        printf("IP addr %d: %s[%x]\n", i+1, inet_ntoa(*(struct in_addr*)hostinfo->h_addr_list[i]) , (*(struct in_addr*)hostinfo->h_addr_list[i]).s_addr);
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = servinfo->s_port;
    address.sin_addr = *(in_addr*)*hostinfo->h_addr_list;
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int result = connect(sockfd, (sockaddr*)&address, sizeof(address));
    assert(result != -1);

    char buffer[128];
    result = read(sockfd, buffer, sizeof(buffer));
    assert(result > 0);
    buffer[result] = '\0';
    printf("the day time is :%s", buffer);
    close(sockfd);

    return 0;
}