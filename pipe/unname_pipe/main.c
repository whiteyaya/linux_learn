#include <stdio.h>
#include <unistd.h>
#include <string.h>
int main()
{
    //给匿名管道创建的文件描述符
    int fd[2];
    char str[] = "hello";
    //创建匿名管道，fd[0]位读端，fd[1]为写端
    pipe(fd);
    if(fork()==0)
    {
        //子进程
        //关闭读端
        close(fd[0]);
        while(1)
        {
            write(fd[1],str,sizeof(str));
            sleep(1);
        }
    }
    else
    {
        //父进程
        //关闭写端
        close(fd[1]);
        char str[64]; 
        while(1)
        {
            read(fd[0],str,sizeof(str));
            printf("father:%s\n",str);
        }
    }
}