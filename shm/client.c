#include"comm.h"

int main()
{
    key_t key = ftok(PATH_NAME,PROJ_ID);
    if(key<0)
    {
        perror("ftok");
        return 1;
    }

    //client直接获取共享内存的shmid
    //不需要创建和删除
    int shmid = shmget(key,SIZE,IPC_CREAT);
    if(shmid<0)
    {
        perror("shmget");
        return 2;
    }

    //关联共享内存
    char* mem = (char*)shmat(shmid,NULL,0);
    char c = 'a';
    int i;
    //每隔一秒写一次
    for(i=0;i<10;i++)
    {
        mem[i]=c;
        mem[i+1]=0;
        c++;
        sleep(1);
    }


    //与共享内存去关联
    shmdt(mem);


    return 0;
}