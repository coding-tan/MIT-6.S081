#include "kernel/types.h"
#include "user/user.h"

#define RD 0 // pipe的读取端
#define WR 1 // pipe的写入端

int main(int argc, char *argv[])
{   
    char buf = 'P'; // 用于传送的字节
    int len_Byte = sizeof(char); //一个字节的长度

    int fd_p2c[2]; // 父进程->子进程
    int fd_c2p[2]; // 子进程->父进程
    pipe(fd_p2c); 
    pipe(fd_c2p);

    int pid = fork();
    int exit_status = 0;

    if(pid < 0)
    {
        fprintf(2, "fork() error!\n");
        close(fd_p2c[RD]);
        close(fd_p2c[WR]);
        close(fd_c2p[RD]);
        close(fd_c2p[WR]);
        exit(1);
    }
    else if(pid == 0) // 子进程
    {
        close(fd_p2c[WR]);
        close(fd_c2p[RD]);

        if(read(fd_p2c[RD], &buf, len_Byte) != len_Byte)
        {
            fprintf(2, "Child read() error!\n");
            exit_status = 1; 
        }
        else
        {
            fprintf(1, "%d: received ping\n", getpid());
        }

        if(write(fd_c2p[WR], &buf, len_Byte) != len_Byte)
        {
            fprintf(2, "Child write() error!\n");
            exit_status = 1; // 标记出错
        }

        close(fd_c2p[WR]);
        close(fd_p2c[RD]);

        exit(exit_status);
    }
    else // 父进程
    {
        close(fd_p2c[RD]);
        close(fd_c2p[WR]);

        if(write(fd_p2c[WR], &buf, len_Byte) != len_Byte)
        {
            fprintf(2, "Parent write() error!\n");
            exit_status = 1; // 标记出错
        }

        if(read(fd_c2p[RD], &buf, len_Byte) != len_Byte)
        {
            fprintf(2, "Parent read() error!\n");
            exit_status = 1;
        }
        else
        {
            fprintf(1, "%d: received pong\n", getpid());
        }

        close(fd_p2c[WR]);
        close(fd_c2p[RD]);

        exit(exit_status);
    }
}