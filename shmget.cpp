#include <bits/stdc++.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;
void shm_read(const char *shm_name) {

    key_t key = ftok(shm_name, 'r');
    assert(key != -1);
    int shmid = shmget(key, 4096, IPC_CREAT);
    char *addr = (char *)shmat(shmid, NULL, SHM_RDONLY);
    while (1) {
        printf("%s\n", addr);
        this_thread::sleep_for(chrono::seconds(1));
    }
    shmdt(addr);
}
void shm_write(string file) { string data = "1"; }
int main() {
    const char *shm_name = "/myshm2";
    thread write = thread(shm_write, shm_name);
    thread read = thread(shm_read, shm_name);
    write.join();
    read.join();
}