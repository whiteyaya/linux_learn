#include"comm.h"

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
    
    //创建套接字，打开网络文件
    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd < 0)
    {
        perror("socket");
        return 1;
    }
    // 发送对象
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);
    // 使用服务
    while (1)
    {
        printf("请输入:\n\t");
        char line[1024];
        fgets(line, sizeof(line), stdin);
        sendto(socketfd, line, strlen(line), 0, (struct sockaddr*)&server, sizeof(server));
        struct sockaddr_in tmp;
        socklen_t len = sizeof(tmp);
        char buffer[1024];
        ssize_t cnt = recvfrom(socketfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&tmp, &len);
        if(cnt > 0)
        {
            buffer[cnt-1] = 0;
            printf("%s\n",buffer);
        }
    }
    return 0;
}



