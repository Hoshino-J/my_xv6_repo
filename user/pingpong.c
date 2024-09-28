#include "kernel/types.h"
#include "user.h"

int main() {
    int f2c[2], c2f[2]; // 定义两个管道,f2c用于父进程向子进程传输数据,c2f用于子进程向父进程传输数据
    char buf[100];
    int parent_pid = getpid(); // 获取父进程PID

    if (pipe(f2c) < 0 || pipe(c2f) < 0) {
        printf("pipe error\n");
        exit(-1);
    }

    int pid = fork();
    if (pid < 0) {
        printf("fork error\n");
        exit(-1);
    }

    if (pid > 0) { // 父进程读c2f管道,写f2c管道
        close(f2c[0]); // 关闭f2c读端
        close(c2f[1]); // 关闭c2f写端

        write(f2c[1], "ping", 5); // 向管道写入数据
        read(c2f[0], buf, sizeof(buf)); // 从子进程接收数据
        printf("%d: received %s from pid %d\n", getpid(), buf, pid); // 这里的pid是子进程的pid
        
        close(f2c[1]);
        close(c2f[0]);
        wait(0); // 等待子进程结束
    } else { // 子进程读f2c管道,写c2f管道
        close(f2c[1]); // 关闭f2c写端
        close(c2f[0]); // 关闭c2f读端

        read(f2c[0], buf, sizeof(buf)); // 从父进程接收数据
        printf("%d: received %s from pid %d\n", getpid(), buf, parent_pid); // 这里的parent_pid是父进程的pid        
        write(c2f[1], "pong", 5); // 向父进程写入数据

        close(f2c[0]);
        close(c2f[1]);
        exit(0);
    }
    return 0;
}
