#include"comm.h"

int main()
{
    key_t key = ftok(PATH_NAME,PROJ_ID);
    if(key<0)
    {
        perror("ftok");
        return 1;
    }
    //创建共享内存
    int shmid = shmget(key,SIZE,IPC_CREAT|IPC_EXCL|0666);
    if(shmid<0)
    {
        perror("shmget");
        return 2;
    }
    //关联到共享内存
    char *mem = (char*)shmat(shmid,NULL,0);
    int i;
    //每秒读一次共享内存
    for(i=0;i<20;i++)
    {
        printf("%s\n",mem);
        sleep(1);
    }


    //与共享内存去关联
    shmdt(mem);

    //释放共享内存
    shmctl(shmid,IPC_RMID,NULL);

    return 0;
}