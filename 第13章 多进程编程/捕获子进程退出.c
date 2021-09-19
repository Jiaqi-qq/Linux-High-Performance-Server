#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

static void handle_chile(int sig) {
    pid_t pid;
    int stat;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		/* 对结束的子进程进行善后处理 */
	}
}