/******************************************************
 * client.cc--网络计算器客户端
 * 用于学习序列化-反序列化传输
 * 运行指令: ./client server_ip server_port
 * 运算符包括： "+ - * / %"
 * 使用前请yum install -y jsoncpp-devel 编译时加 -ljsoncpp
******************************************************/

#include "protocol.hpp"
#include "sock.hpp"

// 运行格式错误
void Usage(string proc)
{
    cout << "Usage: " << proc << " server_ip server_port" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    // 创建套接字
    int sock = Sock::Socket();

    // 连接服务器
    Sock::Connect(sock, argv[1], atoi(argv[2]));

    // while (true)
    // {

    // 业务逻辑
    request_t request;
    memset(&request, 0, sizeof(request));
    cout << "Please Enter Data One# ";
    cin >> request.x;
    cout << "Please Enter operator# ";
    cin >> request.op;
    cout << "Please Enter Data Two# ";
    cin >> request.y;
    

    std::string json_string = SerializeRequest(request);

    ssize_t s = write(sock, json_string.c_str(), json_string.size());

    char buffer[1024];
    s = read(sock, buffer, sizeof(buffer) - 1);
    if (s > 0)
    {
        response_t response;
        buffer[s] = 0;
        std::string str = buffer;
        DeserializeResponse(str, response);

        cout << "code: " << response.code << endl;
        cout << "result: " << response.result << std::endl;
    }

    // }

    return 0;
}