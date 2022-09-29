#include<iostream>
#include<pthread.h>
#include<unistd.h>
using namespace  std;
#define NUM 5
pthread_mutex_t mutex;
pthread_cond_t cond;

void* MasterRun(void* args)
{
    int num = *(int*)args,i;
    for(i=0;i<num;i++)
    {
        pthread_cond_signal(&cond);
        sleep(1);
    }
}
void* WorkerRun(void* args)
{
    int worker_id = *(int*)args;

    
    while(1)
    {
        // 1. 调用的时候，会首先自动释放mtx_,然后再挂起自己
        // 2. 返回的时候，会首先自动竞争锁，获取到锁之后，才能返回
        pthread_cond_wait(&cond,&mutex);
        cout<<worker_id<<" is working"<<endl;
    }
}

int main()
{
    pthread_t master;
    pthread_t workers[NUM];
    int i;
    pthread_mutex_init(&mutex,nullptr);
    pthread_cond_init(&cond,nullptr);

    void *num = new int(NUM);
    pthread_create(&master,nullptr,MasterRun,num);
    for(i=0;i<NUM;i++)
    {
        void *worker_id = new int(i);
        pthread_create(workers+i,nullptr,WorkerRun,worker_id);
    }
    
    for(i=0;i<NUM;i++)
    {
        pthread_join(workers[i],nullptr);
    }
    pthread_join(master,nullptr);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    
    return 0;
}