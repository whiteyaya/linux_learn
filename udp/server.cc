#include"comm.h"

//输入格式错误提示
std::string Usage(std::string proc)
{
    std::cout <<"Usage: "<< proc << " port" << std::endl;
}

// ./server port
int main(int argc, char *argv[])
{
    //判断输入格式
    if(argc != 2){
        Usage(argv[0]);
        return -1;
    }
    //端口号
    uint16_t port = atoi(argv[1]);

    //创建套接字
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "socket create error: " << errno << std::endl;
        return 1;
    }

    // 服务器绑定端口和ip
    struct sockaddr_in local;
    //设置地址族
    local.sin_family = AF_INET;
    //修改至网络字节序考虑大小端
    local.sin_port = htons(port); 
    //INADDR_ANY的作用是只要访问个端口,就数据接收，不考虑ip。
    //INADDR_ANY相当于0.0.0.0
    local.sin_addr.s_addr = INADDR_ANY;
    //绑定
    if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
        std::cerr << "bind error : " << errno << std::endl;
        return 2;
    }

    //服务器端提供服务
    bool quit = false;
    char buffer[NUM];

    while (!quit)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        //接收数据存入缓冲区--buffer
        ssize_t cnt = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
        if (cnt > 0)
        {
            //c字符串结尾置零--'\0'
            buffer[cnt-1] = 0;
            //popen(const char* command,const char* type);
            //可以执行字符串command里的命令--创建子进程利用管道通信
            //type为"r"命令执行的标准输出变为fp--执行结果放入fp
            FILE *fp = popen(buffer, "r");
            std::string echo;
            char line[1024] = {0};
            //将命令执行结果按行读取，追加到echo中
            while(fgets(line, sizeof(line), fp) != NULL)
            {
                echo += line;
            }
            //关闭popen的fp
            pclose(fp);
            //服务器端输出接收到的命令
            std::cout << "client# " << buffer << std::endl;
            //向客户端返回命令执行结果
            sendto(sock, echo.c_str(), echo.size(), 0, (struct sockaddr *)&peer, len);
        }
    }

    return 0;
}

