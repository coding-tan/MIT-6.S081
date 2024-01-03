#include "kernel/types.h"
#include "user/user.h"

#define RD 0 // pipe的读取端
#define WR 1 // pipe的写入端
#define NUM 34

/**
 * @brief 寻找素数
 * @param intput 输入数据
 * @param count 数据大小
 * @return 无返回值
 */
void findprimes(int *intput, int count)
{
   if(count == 0) //当数组没有素数时，退出递归返回上一层调用
    {
        return;
    }

    int prime = *intput; //输入的第一个数2为素数
    int readdata;
    int p[2];
    pipe(p);

    printf("prime %d\n", prime);
    
    if(fork() == 0) //子进程
    {
        close(p[WR]);
        count = 0;

        while(read(p[RD], &readdata, sizeof(int)) != 0)
        {
            if(readdata % prime)
            {
                *intput = readdata;
                intput++;
                count++;
            }
        }

        findprimes(intput - count, count);

        close(p[RD]);
        exit(0);
    }
    else //父进程
    {
        close(p[RD]);
        for(int i = 0; i < count; i++)
        {
            write(p[WR], intput+i, sizeof(int));
        }
        close(p[WR]);
        wait(0);
    }
}

int main(int argv, char *argc[])
{
    int intput[NUM]; 

    for(int i = 0; i < 34; i++) //初始化输入数据，2-35
    {
        intput[i] = i + 2;
    }

    findprimes(intput, NUM);

    exit(0);
}
