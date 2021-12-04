#include <bits/stdc++.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

void mmap_read(const char *shm_name) {
    int len = 4096;
    // 打开文件
    // int fd = open(_file.c_str(), O_RDONLY);
    int shmfd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    assert(shmfd != -1);
    int ret = ftruncate(shmfd, len);
    assert(ret != -1);
    // 读取文件长度
    // int len = lseek(shmfd, 0, SEEK_END);
    // 建立内存映射
    char *share_mem = (char *)mmap(NULL, len, PROT_READ, MAP_SHARED, shmfd, 0);
    assert(share_mem != MAP_FAILED);
    close(shmfd);
    // data用于保存读取的数据
    // char *data = new char[len + 1];
    // 复制过来
    // memcpy(data, addr, len);
    while (1) {
        printf("%s\n", share_mem);
        this_thread::sleep_for(chrono::seconds(1));
    }
    // 解除映射
    munmap(share_mem, len);
    shm_unlink(shm_name);
}
void mmap_write(const char *shm_name) {
    int len = 4096;
    string data = "";
    // int len = data.length();
    // 打开文件
    // int fd = open(file.c_str(), O_RDWR | O_TRUNC, 00777);
    int shmfd = shm_open(shm_name, O_RDWR | O_CREAT, 0666);
    assert(shmfd != -1);
    int ret = ftruncate(shmfd, len);
    assert(ret != -1);
    // lseek将文件指针往后移动file_size-1位
    // lseek(shmfd, 4096, SEEK_END);
    // 从指针处写入一个空字符；mmap不能扩展文件长度，这里相当于预先给文件长度，准备一个空架子
    // write(fd, "", 1);
    // 使用mmap函数建立内存映射
    char *addr = (char *)mmap(NULL, len, PROT_WRITE, MAP_SHARED, shmfd, 0);
    // 内存映射建立好了，此时可以关闭文件了
    close(shmfd);
    // 把data复制到addr里
    int start = 0;
    while (1) {
        data += "1";
        memcpy(addr, data.c_str(), data.length());
        // start += len;
        this_thread::sleep_for(chrono::seconds(1));
    }
    // 解除映射
    munmap(addr, len);
}
int main() {
    // cat /dev/shm/myshm
    const char *file = "/myshm";
    thread write = thread(mmap_write, file);
    thread read = thread(mmap_read, file);
    write.join();
    read.join();
}