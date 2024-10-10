#include "kernel/types.h"
#include "user.h"

int main() {
    int f2c[2];         // 定义一个管道,f2c用于父进程向子进程传输数据
    int parent_pid = getpid(); // 获取父进程PID

    // 创建管道,并检查是否成功
    if (pipe(f2c) < 0) {
        printf("pipe error\n");
        exit(-1);
    }

    int pid = fork();   // 创建子进程
    if (pid < 0) {
        printf("fork error\n");
        exit(-1);
    }

    if (pid > 0) { // 父进程写f2c管道
        close(f2c[0]);  // 关闭f2c读端

        // 向子进程传递父进程的pid
        write(f2c[1], (char*)&parent_pid, sizeof(parent_pid));      // 将父进程的pid写入管道
        
        close(f2c[1]);  // 关闭f2c写端

        wait(0); // 等待子进程结束后再打印避免父子进程的printf语句交替进行
        printf("%d: received pong from pid %d\n", parent_pid, pid); // 这里的pid由fork函数返回值提供

    } else {    // 子进程读f2c管道
        close(f2c[1]);  // 关闭f2c写端
        
        // 从管道中读取父进程的pid
        int received_parent_pid;
        read(f2c[0], (char*)&received_parent_pid, sizeof(received_parent_pid)); // 从管道中读取父进程的pid
        printf("%d: received ping from pid %d\n", getpid(), received_parent_pid);        

        close(f2c[0]);  // 关闭f2c读端

        exit(0);    // 子进程退出
    }
    return 0;
}
