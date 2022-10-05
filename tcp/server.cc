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
    // 创建监听套接字 
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sock < 0) 
    {
        std::cerr <<"socket error: " << errno << std::endl;
        return 2;
    }
    // 服务器绑定端口和ip
    struct sockaddr_in local;
    //初始化清零
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(argv[1]));//修改至网络字节序考虑大小端
    local.sin_addr.s_addr = INADDR_ANY;//INADDR_ANY的作用是只要访问个端口,就数据接收，不考虑ip。
    if(bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0)
    {
        std::cerr << "bind error: " << errno << std::endl;
        return 3;
    }
    //因为tcp是面向连接的,在通信前,需要建连接,然后才能通信。
    //设置套接字是Listen状态, 本质是允许用户连接
    const int back_log = 5;
    // listen 设置socket文件的状态,允许用户来连接我
    if(listen(listen_sock, back_log) < 0)
    {
        std::cerr << "listen error" << std::endl;
        return 4;
    }
    std::cout << "waiting connect..." << std::endl;
    while( 1 ) 
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
        //创建线程、进程无上限,当客户链接来了，我们才给客户创建进程/线程
        //构建一个任务
        Task t(new_sock);
        //将任务push到后端的线程池即可
        ThreadPool<Task>::GetInstance()->PushTask(t);
    }
    return 0;
}
