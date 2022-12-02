#include "comm.hpp"

int main()
{
    int msgid = CreateMsgQueue();

    char buf[1024] = {0};
    while(1)
    {
        RecvMsgQueue(msgid, CLIENT_TYPE, buf);
        if(strcasecmp("quit", buf) == 0)
            break;
        printf("client# %s\n", buf);

        printf("Please enter# ");
        fflush(stdout);
        ssize_t s = read(0, buf, sizeof(buf));
        if(s>0)
        {
            buf[s-1]=0;
            SendMsgQueue(msgid, SERVER_TYPE, buf);
            printf("send done, wait recv...\n");
        }
    }
    DestoryMsgQueue(msgid);

    
    return 0;
}
