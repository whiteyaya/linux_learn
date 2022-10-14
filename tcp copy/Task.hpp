/***************************************************
 * 简易MyShell任务类--Task.hpp
 * 推送进来的任务存放在任务队列当中
 * 使用线程池处理任务->SigThreadPool.hpp
 ***************************************************/

#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
namespace ns_task
{
    class Task
    {
    private:
        int _sock;

    public:
        Task() : _sock(-1) {}
        Task(int sock) : _sock(sock) {}

        //执行任务
        int Run()
        {
            while (1)
            {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                //通过网络套接字读取传入的内容存入buffer
                ssize_t s = read(_sock, buffer, sizeof(buffer) - 1);

                // std::cout<<s<<std::endl;

                if (s > 0)
                {
                    //将获取的内容当成字符串，结尾置0
                    buffer[s] = 0;
                    //输出客户发送的指令
                    std::cout << "client" << _sock << "# " << buffer << std::endl;

                    // popen(const char* command,const char* type);
                    //可以执行字符串command里的命令--创建子进程利用管道通信
                    // type为"r"命令执行的标准输出变为fp--执行结果放入fp
                    FILE *fp = popen(buffer, "r");

                    std::string echo = "\n";
                    char line[1024] = {0};

                    //将命令执行结果按行读取，追加到echo中
                    while (fgets(line, sizeof(line), fp) != NULL)
                    {
                        echo += line;
                    }
                    //应答--向客户端返回命令执行结果
                    write(_sock, echo.c_str(), echo.size());

                    //关闭popen的fp
                    pclose(fp);
                }
                //客户端退出
                else if (s == 0)
                {
                    std::cout << "client " << _sock << " quit" << std::endl;
                    close(_sock);
                    return 0;
                }
                else
                {
                    std::cerr << "read error" << std::endl;
                }
            }

            close(_sock);
            return 0;
        }
        ~Task() {}
        int operator()()
        {
            return Run();
        }
    };
}
