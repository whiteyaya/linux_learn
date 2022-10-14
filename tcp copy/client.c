/***************************************************
 * 简易MyShell客户端--client.c
 * 运行时请添加服务器端口号、ip--./clitne 127.0.0.1 8081
 * 客户端可以输入shell命令
 * --比如 ls -l、pwd...
 * 输入"quit"退出客户端程序
***************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>

void Usage(char* proc)
{
    printf("Usage:%s server_ip server_port\n",proc);
}
// ./client server_ip server_port
int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        return 1;
    }
    char* svr_ip = argv[1];
    uint16_t svr_port = (uint16_t)atoi(argv[2]);

    //创建socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket error!\n");
        return 2;
    }

    //bind
    struct sockaddr_in server;
    bzero(&server, sizeof(server));//全部初始化为0。
    
    server.sin_family = AF_INET;
    //inet_addr做两件事情
    //1. 将点分十进制的字符串风格的IP，转化成为4字节IP
    //2. 将4字节由主机序列转化成为网络序列
    server.sin_addr.s_addr = inet_addr(svr_ip); 
    server.sin_port = htons(svr_port); // 主机转网络字节序

    //发起链接
    if(connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        printf("connect server failed !\n");
        return 3;
    }
    
    //链接成功
    printf("connect success !\n");

    // 业务请求
    while(1)
    {
        printf("MyShell # \n");
        char buffer[1024];
        fgets(buffer, sizeof(buffer)-1, stdin);
        buffer[strlen(buffer)-1]=0;
        if(strcmp(buffer,"quit")==0)
        {
            return 1;
        }
        write(sock, buffer, strlen(buffer));
        ssize_t s = read(sock, buffer, sizeof(buffer)-1);
        
        if(s > 0)
        {
            if(s==1)
            {
                printf("command not found\n");
                continue;
            }
            buffer[s] = 0;
            printf("%s\n",buffer);
        }
    }

    return 0;
}
