#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// fmtname函数用于获取文件名
// fmtname函数的参数是文件路径，返回值是文件名
char* fmtname(char *path) 
{   
    static char buf[DIRSIZ+1]; //定义静态变量buf，用于存储文件名
    char *p; //定义指针p，用于指向文件名

    // Find first character after last slash.
    // 找到路径中最后一个斜杠后面的第一个字符。
    for(p=path+strlen(path); p >= path && *p != '/'; p--);
    p++;

    // Return blank-padded name.
    // 返回空白填充的名称。
    if(strlen(p) >= DIRSIZ)
    return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    // 确保 buf 数组中存储的文件名是以 null 结尾的 C 字符串,防止strcmp比较函数失效
    buf[strlen(p)] = '\0'; 
    return buf;
}

void find(char *path, char *filename)
{
    char buf[512], *p; //定义buf数组，用于存储文件路径，定义指针p，用于指向文件路径
    int fd;
    struct dirent de; //定义目录项的结构体de，用于存储目录项的信息包括文件名和文件的inode号
    struct stat st; //定义文件状态结构体st，用于存储文件的状态信息包括文件类型、文件大小、inode号等

    //open函数用于打开文件，返回文件描述符fd，错误返回-1
    //open函数的第一个参数是文件路径，第二个参数是打开文件的方式，0表示只读方式打开
    if((fd = open(path, 0)) < 0) 
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    //fstat函数用于获取文件的状态，返回文件状态结构体st，错误返回-1
    //fstat函数的第一个参数是文件描述符，第二个参数是文件状态结构体
    if(fstat(fd, &st) < 0)
    { // 获取文件的状态
        fprintf(2, "find: cannot stat %s\n", path); 
        close(fd);
        return;
    }
    //switch语句用于判断文件类型
    switch(st.type){
        case T_FILE: // 如果是文件类型，判断文件名是否与指定文件名相同，相同则打印文件路径
            if(strcmp(fmtname(path), filename) == 0)
            {
                printf("%s\n", path);
            }
            break;

        case T_DIR: // 如果是目录类型，遍历目录下的所有文件，递归调用find函数
            // 判断路径长度是否超过512，超过则报错
            // DIRSIZ是目录项的最大长度，定义在kernel/fs.h中
            // 第一个 + 1 是因为路径后面要加上一个"/"
            // 第二个 + 1 是因为路径后面要加上一个"\0"
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
            {
                printf("ls: path too long\n");
                break;
            }
            strcpy(buf, path);
            // p指向路径的最后一个字符
            p = buf+strlen(buf);
            *p++ = '/';
            // 读取目录项，判断目录项是否为空，是否为"."或者".."，不是则递归调用find函数
            while(read(fd, &de, sizeof(de)) == sizeof(de))
            {
                if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                {
                    continue;
                }
                // 将目录项的文件名拼接到路径后面
                memmove(p, de.name, DIRSIZ);
                // 确保 p 数组中存储的文件名是以 null 结尾的 C 字符串,防止strcmp比较函数失效
                p[DIRSIZ] = '\0';
                // 递归调用find函数
                find(buf, filename);
            }
            break;
    }
    close(fd);

}

int main(int argc, char *argv[])
{
    if(argc != 3) // 输入参数不为3 报错
    {
        fprintf(2, "Usage: find <path> <name>\n");
        exit(1);
    }
    //调用find函数查找指定目录下特定名称的所有文件
    find(argv[1], argv[2]);
    
    exit(0);
}