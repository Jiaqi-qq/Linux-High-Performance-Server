#include <fcntl.h>

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);        // 获取文件描述符旧的状态标志
    int new_option = old_option | O_NONBLOCK;   // 设置非阻塞
    fcntl(fd, F_SETFL, new_option);            
    return old_option;                          // 返回旧状态
}