#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{   
    //if(argc <= 1) //虽然能通过测试，但是输入sleep 10 10就不会提示错误
    if(argc != 2)
    {
        fprintf(2, "usage: sleep <time>\n");
        exit(1); //exit(1)表示错误退出
    }
    else
    {
        sleep(atoi(argv[1]));
    }
    exit(0);
}

