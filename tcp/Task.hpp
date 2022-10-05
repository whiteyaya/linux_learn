#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>

namespace ns_task
{
    class Task
    {
    private:
        int _sock;

    public:
        Task() : _sock(-1) {}
        Task(int sock) : _sock(sock)
        {
        }
        //执行任务
        int Run()
        {
            while (1)
            {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                //通过网络套接字读取传入的内容
                ssize_t s = read(_sock, buffer, sizeof(buffer) - 1);
                // std::cout<<s<<std::endl;
                if (s > 0)
                {
                    // std::cout<<s<<std::endl;
                    buffer[s] = 0; //将获取的内容当成字符串
                    std::cout << "client"<<_sock<<"# " << buffer << std::endl;
                    //拉取逻辑
                    std::string echo_string = "get:";
                    echo_string += buffer;
                    write(_sock, echo_string.c_str(), echo_string.size());
                }
                else if (s == 0)
                {
                    std::cout << "client "<<_sock<<" quit" << std::endl;
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
