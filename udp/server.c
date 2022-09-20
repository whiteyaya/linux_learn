#include<stdio.h>
#include<unistd.h>
#include<error.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define NUM 1024
//定义端口号
const uint16_t port = 8080;

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
    
    //服务器运转
    while (!quit)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        printf("waiting>>>\n");
        //通过发送字符串进行通信
        ssize_t cnt = recvfrom(socketfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
         printf("get>>>\n");
        if (cnt > 0)
        {
            buffer[cnt] = 0;
            char re [1024]= "hello\n";
            printf("client:%s\n",buffer);
           // std::cout << "client# " << buffer << std::endl;

            sendto(socketfd, re, sizeof(re), 0, (struct sockaddr *)&peer, len);
        }
    }

    return 0;
}
