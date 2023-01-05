/****************************************
 * http.cc--用于学习http请求与响应
 * 运行时设置端口号 ./http 8088
 * 运行之后使用浏览器向服务器对应端口发起连接
 * 能够得到一串字符回应
 * 其中wwwroot 就叫做web根目录，wwwroot目录下防止的内容，都叫做资源！！
 * wwwroot目录下的index.html就叫做网站的首页


 ****************************************/
#include "sock.hpp"
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>

// 定义响应报文大小
#define SIZE 1024*10
// web根目录
#define WWWROOT "./wwwroot/"
// 网站首页
#define HOME_PAGE "index.html"

// 程序运行错误提示
void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " port" << std::endl;
}

// 重定向演示函数--会被浏览器保存，注意删除
void *HandlerHttpMoved(void *args)
{
    int sock = *(int*)args;
    delete (int*)args;
    pthread_detach(pthread_self());

    char buffer[SIZE];
    memset(buffer, 0 , sizeof(buffer));
    // 获取请求报文
    ssize_t s = recv(sock, buffer, sizeof(buffer), 0);

    // 配置响应报文
    // 301--永久重定向
    std::string http_response = "http/1.0 301 Permanently moved\n";
    http_response += "Location: http://qq.com/\n";
    http_response += "\n";

    // 发送响应报文
    send(sock, http_response.c_str(), http_response.size(), 0);

    close(sock);
    return nullptr;
}

// 线程处理http请求函数
void *HandlerHttpRequest(void *args)
{
    //Http协议--本质是根据协议内容进行文本分析
    int sock = *(int*)args;
    delete (int*)args;
    pthread_detach(pthread_self());

    char buffer[SIZE];
    memset(buffer, 0 , sizeof(buffer));
    // 获取请求报文
    ssize_t s = recv(sock, buffer, sizeof(buffer), 0);
    
    if(s > 0)
    {
        buffer[s] = 0;
        // 打印请求报文
        // std::cout  << buffer; 

        std::string html_file = WWWROOT;
        html_file += HOME_PAGE;

        std::string http_response;
        // Body的内容--首页
        std::ifstream in(html_file);
        if (!in.is_open())
        {
            http_response = "http/1.0 404 NOT FOUND\n";
            http_response += "Content-Type: text/html; charset=utf8\n";
            http_response += "\n";
            http_response += "<html><p>你访问的资源不存在</p></html>";
        }
        else
        {
            // 使用stat获取文件属性
            struct stat st;
            stat(html_file.c_str(), &st);

            // 响应报文
            // 响应首行--[版本号] + [状态码] + [状态码解释]
            http_response = "http/1.0 200 ok\n";
            http_response += "Content-Type: text/html; charset=utf8\n";
            http_response += "Content-Length: ";
            http_response += std::to_string(st.st_size);
            http_response += "\n";

            //Set-Cookie: 向浏览器设置一个cookeie
            http_response += "Set-Cookie: id=11111\n";
            http_response += "Set-Cookie: password=2222\n";
            http_response += "\n";

            // Body部分
            std::string content;
            std::string line;
            while (std::getline(in, line))
            {
                content += line;
            }
            http_response += content;
            in.close();
        }
        send(sock, http_response.c_str(), http_response.size(), 0);
    }

    close(sock);
    return nullptr;
}

int main(int argc, char *argv[])
{
    if( argc != 2 )
    {
        Usage(argv[0]);
        exit(1);
    }

	//短链接版本(即一次请求,一次响应,每次都关闭套接字)
    uint16_t port = atoi(argv[1]);
    int listen_sock = Sock::Socket();
    Sock::Bind(listen_sock, port);
    Sock::Listen(listen_sock);

    std::cout<<"Start Running..."<<std::endl;

    // 服务器后台循环接收请求--创建线程处理
    for( ; ; )
    {
        int sock = Sock::Accept(listen_sock);
        if(sock > 0)
        {
            std::cout<<"Get ["<<sock<<"]"<<std::endl;
            pthread_t tid;
            int *parm = new int(sock);
            pthread_create(&tid, nullptr, HandlerHttpRequest, parm);
            // for test--演示301重定向
            // pthread_create(&tid, nullptr, HandlerHttpMoved, parm);
        }
    }
}
