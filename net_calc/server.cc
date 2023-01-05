/******************************************************
 * server.cc--网络计算器服务器端
 * 用于学习序列化-反序列化传输
 * 多线程处理 
 * 运行指令: ./server port
 * 结束指令: ctrl+c
 * 使用前请yum install -y jsoncpp-devel 编译时加 -ljsoncpp
 * 
******************************************************/


#include <pthread.h>
#include "protocol.hpp"
#include "sock.hpp"

//调用错误信息
static void Usage(string proc)
{
    cout << "Usage: " << proc << " port" << endl;
    exit(1);
}

//线程执行函数 args--通信套接字
void *HandlerRequest(void *args)
{
    int sock = *(int *)args;
    delete (int *)args;

    //线程分离
    pthread_detach(pthread_self());

    // 读取请求
    char buffer[1024];
    request_t request;
    ssize_t s = read(sock, buffer, sizeof(buffer) - 1);

    if (s > 0)
    {
        buffer[s] = 0;
        cout << "get a new request: " << buffer << endl;
        std::string str = buffer;
        //反序列化请求
        DeserializeRequest(str, request); 

        // 构建返回响应
        response_t response = {0, 0};
         // 分析请求 && 计算结果
        switch (request.op)
        {
        case '+':
            response.result = request.x + request.y;
            break;
        case '-':
            response.result = request.x - request.y;
            break;
        case '*':
            response.result = request.x * request.y;
            break;
        case '/':
            if (request.y == 0)
                response.code = -1; //代表除0
            else
                response.result = request.x / request.y;
            break;
        case '%':
            if (request.y == 0)
                response.code = -2; //代表模0
            else
                response.result = request.x % request.y;
            break;
        default:
            response.code = -3; //代表请求方法异常
            break;
        }
        cout << "request: " << request.x << request.op << request.y << endl;

        //序列化返回
        std::string send_string = SerializeResponse(response);   
        write(sock, send_string.c_str(),send_string.size());     
        cout << "服务结束: " << send_string << endl;
        // }
    }

    // 5. 关闭链接
    close(sock);
}


int main(int argc, char *argv[])
{
    if (argc != 2)
        Usage(argv[0]);
    uint16_t port = atoi(argv[1]);

    //监听套接字--创建、绑定、监听
    int listen_sock = Sock::Socket();
    Sock::Bind(listen_sock, port);
    Sock::Listen(listen_sock);

    //多线程处理
    for (;;)
    {
        //获取新通信套接字
        int sock = Sock::Accept(listen_sock);
        if (sock >= 0)
        {
            cout << "get a new client..." << endl;
            //创建线程
            int *pram = new int(sock);
            pthread_t tid;
            pthread_create(&tid, nullptr, HandlerRequest, pram);
        }
    }

    return 0;
}