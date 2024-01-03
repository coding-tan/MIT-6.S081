#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    char *lineSplit[MAXARG], *p; // 用于存储命令行参数
    int i, count = 0, k, m = 0;
    char block[MAXARG], buf[MAXARG];
    p = buf;

    if(argc <= 1){
        fprintf(2, "xargs: no command given\n");
        exit(1);
    }

    for(i=1; i<argc; i++){
        printf("%s ", argv[i]);
    }

    for(i=1; i<argc; i++){ // 将命令行参数存储到lineSplit中, 用于exec函数调用
        lineSplit[count++] = argv[i]; // lineSplit[0]存储的是命令行参数中的命令
    }  
    lineSplit[i-1] = 0; // exec函数调用需要以NULL结尾
    
    while((k = read(0, block, sizeof(block))) > 0){ // 从标准输入读取数据到block中
        for(i = 0; i < k; i++){
            printf("block[%d] = %c\n", i, block[i]);
            if(block[i] == '\n'){ // 如果读取到换行符，则创建子进程
                printf("block[%d] = \\n\n", i);
                buf[m] = 0;
                lineSplit[count++] = p; // 将buf中的数据存储到lineSplit中
                lineSplit[count] = 0;
                m = 0; 
                p = buf; 
                count = argc - 1; 
                if(fork() == 0){ // 创建子进程
                    printf("fork success\n");
                    exec(lineSplit[0], lineSplit); // 执行命令
                    exit(0);
                }
                wait(0);
            }
            else if(block[i] == ' '){
                buf[m++] = 0;
                lineSplit[count++] = p; // 将buf中的数据存储到lineSplit中
                p = &buf[m];
            }
            else{
                buf[m++] = block[i];
            }
        }
    }
    
    exit(0);
}