/***************************************************
 * 简易MyShell服务器端--server.c
 * 运行时请添加端口号--./server 8081
 * 使用ctrl+c退出
***************************************************/

#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include "Task.hpp"
#include "SigThreadPool.hpp"

using namespace ns_threadpool;
using namespace ns_task;

//运行格式错误
void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " port" << std::endl;
}
// ./server 8081
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        return 1;
    }

    // 创建listen监听套接字 
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sock < 0) {
        std::cerr <<"socket error: " << errno << std::endl;
        return 2;
    }
    
    // bind--服务器绑定端口和ip
    struct sockaddr_in local;
    //初始化清零
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    //修改至网络字节序考虑大小端
    local.sin_port = htons(atoi(argv[1]));
    //INADDR_ANY表示所有ip均可访问
    local.sin_addr.s_addr = INADDR_ANY;
    if(bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0)
    {
        std::cerr << "bind error: " << errno << std::endl;
        return 3;
    }

    //设置套接字Listen状态, 允许用户连接
    const int back_log = 5;
    if(listen(listen_sock, back_log) < 0)
    {
        std::cerr << "listen error" << std::endl;
        return 4;
    }

    //提供服务
    std::cout << "server running..." << std::endl;
    while(1) 
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        //accept获取新套接字用于通信，peer为输出型参数，接收对方的地址对象信息
        int new_sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
        if(new_sock < 0)
        {
            continue;
        }

        //取出对方地址信息
        uint16_t cli_port = ntohs(peer.sin_port);//网络字节序转为主机字节序
        std::string cli_ip = inet_ntoa(peer.sin_addr);//将网络字节序转为点分十进制
        std::cout << "get a new link -> : [" << cli_ip << ":" << cli_port <<"]# " << new_sock << std::endl;

        //构建一个任务对象
        Task t(new_sock);
        //将任务push到后端的线程池
        ThreadPool<Task>::GetInstance()->PushTask(t);
    }
    return 0;
}
