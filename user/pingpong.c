#include "kernel/types.h"
#include "user.h"

int main() {
    int c2f[2], f2c[2], p2c[2];  // 定义三个管道
    char buf[100];
    int parent_pid = getpid(); // 获取父进程PID

    if (pipe(c2f) < 0 || pipe(f2c) < 0 || pipe(p2c) < 0) {
        printf("pipe error\n");
        exit(-1);
    }

    int pid = fork();
    if (pid < 0) {
        printf("fork error\n");
        exit(-1);
    }

    if (pid > 0) { // 父进程
        close(c2f[0]); // 关闭读端
        close(f2c[1]); // 关闭写端
        close(p2c[0]); // 关闭子进程读端

        write(c2f[1], "ping", 5); // 向管道写入数据
        write(p2c[1], &parent_pid, sizeof(parent_pid)); // 传递父进程PID给子进程
        read(f2c[0], buf, sizeof(buf)); // 从子进程接收数据
        printf("%d: received %s from pid %d\n", getpid(), buf, pid); // 这里的pid是子进程的pid
        
        close(c2f[1]);
        close(f2c[0]);
        close(p2c[1]);
        wait(0); // 等待子进程结束
    } else { // 子进程
        close(c2f[1]); // 关闭写端
        close(f2c[0]); // 关闭读端
        close(p2c[1]); // 关闭父进程写端

        read(c2f[0], buf, sizeof(buf)); // 从父进程接收数据
        read(p2c[0], &parent_pid, sizeof(parent_pid)); // 从父进程接收PID
        printf("%d: received %s from pid %d\n", getpid(), buf, parent_pid); // 这里的parent_pid是父进程的pid
        
        write(f2c[1], "pong", 5); // 向父进程写入数据

        close(c2f[0]);
        close(f2c[1]);
        close(p2c[0]);
        exit(0);
    }
    return 0;
}
