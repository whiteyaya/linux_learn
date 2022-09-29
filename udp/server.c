#include"comm.h"
#define NUM 1024
const uint16_t port = 8080;//定义端口号
int main()
{
    //创建套接字
    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(socketfd < 0)
    {
        perror("socketfd");
        return 1;
    }
    //绑定端口和ip
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    //local.sin_addr.s_addr = inet("175.178.40.2");
    local.sin_addr.s_addr = INADDR_ANY;
    if(bind(socketfd, (struct sockaddr*)&local,sizeof(local)) < 0)
    {
        perror("bind");
        return 2;
    }
    int quit = 0;
    char buffer[NUM];
    int rc_fd = open("receve.txt",O_WRONLY|O_CREAT|O_APPEND,0666);
    //服务器运转
    while (!quit)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        printf("waiting...\n");
        //接收数据
        ssize_t cnt = recvfrom(socketfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
        if (cnt > 0)
        {
            buffer[cnt-1] = 0;
            write(rc_fd, buffer, strlen(buffer));//将数据写入文件夹
            write(rc_fd, "\n", 1);
            printf("client say:\n\t%s\n",buffer);//打印接收数据
            char re [1024]= "receve success!\n";//向客户端反馈
            sendto(socketfd, re, strlen(re), 0, (struct sockaddr *)&peer, len);
        }
    }

    return 0;
}
