#include <cstdio>
#include <cstdlib>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

union semun {
    int val;
    semid_ds *buf;
    unsigned short int *array;
    seminfo *__buf;
};
/* op为-1时执行P操作，op为1时执行V操作 */
void pv(int sem_id, int op) {
    sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = op;
    sem_b.sem_flg = SEM_UNDO;
    semop(sem_id, &sem_b, 1);
}

int main(int argc, char *argv[]) {
    int sem_id = semget(IPC_PRIVATE, 1, 0666);

    semun sem_un;
    sem_un.val = 1;
    semctl(sem_id, 0, SETVAL, sem_un);

    pid_t id = fork();
	if(id < 0) {
        return 1;
    }
	else if(id == 0) {
        printf("child try to get binary sem\n");
		/* 在父、子进程间共享IPC_PRIVATE信号量的关键就在于二者都可以操作该信号量的标识符sem_id */
        pv(sem_id, -1);
        printf("child get the sem and would release it after 5 seconds\n");
        sleep(5);
        pv(sem_id, 1);
        exit(0);
    }
	else {
        printf("parent try to get binary sem\n");
        pv(sem_id, -1);
		printf("parent get the sem and would release it after 5 seconds\n");
        sleep(5);
        pv(sem_id, 1);
    }

    waitpid(id, nullptr, 0);
    semctl(sem_id, 0, IPC_RMID, sem_un); /* 删除信号量 */
    return 0;
}