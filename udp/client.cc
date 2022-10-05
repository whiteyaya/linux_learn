#include"comm.h"

//输入格式错误提示
void Usage(std::string proc)
{
    std::cout << "Usage: \n\t" << proc << " server_ip server_port" << std::endl;
}

// ./client server_ip server_port

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        return 0;
    }

    //创建套接字，打开网络文件
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "socket error : " << errno << std::endl;
        return 1;
    }
    //客户端不需要显式的bind，一般是由OS用的是随机端口的方式自动bind
    //地址对象--存入服务器端的port和ip
    struct sockaddr_in server;
    server.sin_family = AF_INET; //地址族
    server.sin_port = htons(atoi(argv[2])); //port
    server.sin_addr.s_addr = inet_addr(argv[1]); //ip

    // 使用服务
    while (1)
    {
        std::cout << "MyShell # ";
        char line[1024];
        //阻塞式等待输入
        fgets(line, sizeof(line), stdin);
        //发送从stdin读取的数据给服务器
        sendto(sock, line, strlen(line), 0, (struct sockaddr*)&server, sizeof(server));
        //tmp储存输出型参数--接收对方的地址对象信息
        struct sockaddr_in tmp;
        socklen_t len = sizeof(tmp);
        char buffer[1024];
        //接收服务器返回值存入buffer
        ssize_t cnt = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&tmp, &len);
        if(cnt > 0)
        {
            buffer[cnt] = 0;
            std::cout << buffer << std::endl;
        }
    }

    return 0;
}
