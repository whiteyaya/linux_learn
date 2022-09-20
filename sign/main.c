#include<stdio.h>
#include<signal.h>
#include<unistd.h>


void handler(int sig)
{
    printf("signal:%d\n",sig);
}
int main()
{
    signal(2,handler);
    while(1)
    {
        printf("runing......\n");
        sleep(1);
    }
    return 0;
}