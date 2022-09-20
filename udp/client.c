#include<stdio.h>
#include<unistd.h>
#include<error.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

void Usage(char* proc)
{
    printf("%s\n server_ip server_port\n",proc);
}

// ./udp_client server_ip server_port
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        return 0;
    }
    
    // 1. 创建套接字，打开网络文件
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return 1;
    }

     /*客户端需要显式的bind吗?
       首先，客户端必须也要有ip和port,但是，客户端不需要显式的bind！一旦显示bind，就必须明确，client要和哪一个port关联
       client指明的端口号，在client端一定会有吗？？有可能被占用，被占用导致client无法使用
       server要的是port必须明确，而且不变，但client只要有就行！一般是由OS自动给你bind()
       就是client正常发送数据的时候，OS会自动给你bind，采用的是随机端口的方式！*/

    // 发送对象
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    //printf("%d\n",atoi(argv[2]));
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);

    // 使用服务
    while (1)
    {
        printf("MyShell $");
        char line[1024];
        fgets(line, sizeof(line), stdin);
        //printf("%s\n",line);
        sendto(sock, line, sizeof(line), 0, (struct sockaddr*)&server, sizeof(server));

        //此处tmp就是一个”占位符“
        struct sockaddr_in tmp;
        socklen_t len = sizeof(tmp);
        char buffer[1024];
        ssize_t cnt = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&tmp, &len);
        if(cnt > 0)
        {
            //在网络通信中，只有报文大小，或者是字节流中字节的个数，没有C/C++字符串这样的概念。
            buffer[cnt] = 0;
            printf("%s\n",buffer);
        }
    }

    return 0;
}



