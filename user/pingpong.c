#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int n;

    int p1[2]; //定义存放管道的read/write文件描述符的数组
    int p2[2];
    pipe(p1); //创建管道 p1父进程，p2子进程
    pipe(p2);

    if(fork() == 0)
    {
       n = read(p1[0], "1", 1);
       if(n == 1)
       {
            printf("%d: received ping\r\n", getpid());
            write(p2[1], "1", 1);
       }
    }
    else
    {
        write(p1[1], "1", 1);
        n = read(p2[0], "1", 1);
        if(n == 1)
        {
            printf("%d: received pong\r\n", getpid());
        }
    }

    exit(0);
}