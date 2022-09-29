#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define NUM 5

void* thread_run(void* argc)
{
    printf("我是[%d]号线程，我的pthread_t id是[0x%x]\n",argc,pthread_self());

}
int main()
{
    pthread_t tid[NUM];
    int i;
    for(i=0;i<NUM;i++)
    {
        pthread_create(tid+i,NULL,thread_run,(void*)i);
        sleep(1);
    }
    printf("我是主线程，我的pthread_t id是[0x%x]\n",pthread_self());


    return 0;
}