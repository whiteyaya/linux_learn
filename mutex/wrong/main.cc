#include<iostream>
#include<pthread.h>
#include<unistd.h>
using namespace  std;
#define NUM 5

int tickets = 1000;

void* ThreadRun(void* args)
{
    int i =*(int*)args;
    delete (int *)args;
    int t=0;
    while(1)
    {
        if(tickets>0)
        {
            usleep(1000);
            tickets--;
            t++;
            
        } 
        else{
            break;
        }
    }
    cout<<"i am "<<i<<" , i get "<<t<<"tickets"<<endl;
    cout<<"rest:"<<tickets<<endl;
    
}
int main()
{

    pthread_t tid[NUM];
    int i;
    for(i=0;i<NUM;i++)
    {
        void *id = new int(i);
        pthread_create(tid+i,nullptr,ThreadRun,id);
        //sleep(1);
    }
    for(i=0;i<NUM;i++)
    {
        pthread_join(tid[i],nullptr);
    }
    
    //pthread_join(tid,nullptr);
    return 0;
}