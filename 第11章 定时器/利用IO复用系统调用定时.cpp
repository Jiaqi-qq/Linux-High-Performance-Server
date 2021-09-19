#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sys/epoll.h>

#define MAX_EVENT_NUMBER 1024
static int epollfd;
static epoll_event events[MAX_EVENT_NUMBER];

#define TIMEOUT 5000

int main() {
    int timeout = TIMEOUT;
    time_t start = time(nullptr);
    time_t end = time(nullptr);
    while(1) {
        printf("the timeout is now %d mil-seconds\n", timeout);
        start = time(nullptr);
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, timeout);
        if((number < 0) && (errno != EINTR)) {
            printf("epoll failure\n");
            break;
        }

        /* 如果epoll_wait成功返回0,则说明超时时间到，此时便可处理定时任务，并重置定时时间 */
        if(number == 0) {
            timeout = TIMEOUT;
            continue;
        }

        end = time(nullptr);
        /* 如果epoll_wait的返回值大于0,则本次epoll_wait调用持续的时间是 (end-start)*1000 ms，我们需要将定时时间timeout减去这段时间，以获得下次epoll_wait调用的超时参数 */
        timeout -= (end - start) * 1000;
        /* 重新计算之后的timeout值可能等于0,说明本次epoll_wait调用返回时，不仅有文件描述符就绪，而且其超时时间也刚好到达，此时我们也要处理定时任务，并重置定时时间 */
        if(timeout <= 0) {
            timeout = TIMEOUT;
        }

        // handle connections
    }
}