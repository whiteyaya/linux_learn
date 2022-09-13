#include <stdio.h>
#include <unistd.h>
#include <string.h>
int main()
{
    int fd[2];
    char str[] = "hello";
    pipe(fd);
    if(fork()==0)
    {
        //子进程
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
        close(fd[1]);
        char str[64]; 
        while(1)
        {
            read(fd[0],str,sizeof(str));
            printf("father:%s\n",str);
        }
    }
}