/*************************************************************
 * 消息队列：简易四则运算器
 * server.cc--服务端源代码
 * 先执行server再执行client
 * 运行后等待client端输入
 * 程序退出与消息队列的清除由client端控制
 *************************************************************/

#include "comm.hpp"

int main()
{
    int msgid = CreateMsgQueue();
    std::cout << "Server start..." << std::endl;

    msg_buf buf;

    // 服务循环
    while (1)
    {
        std::cout << "Waiting for client..." << std::endl;

        buf.mtype = CLIENT_TYPE;
        RecvMsgQueue(msgid, buf);

        if (buf.code == -999)
        {
            std::cout << "quit" << std::endl;
            break;
        }

        // 默认code=0:success
        buf.code = 0;
        std::cout << "get client:" << buf.source_pid << std::endl;
        // 运算逻辑
        switch (buf.op)
        {
        case '+':
            buf.result = buf.x + buf.y;
            break;
        case '-':
            buf.result = buf.x - buf.y;
            break;
        case '*':
            buf.result = buf.x * buf.y;
            break;
        case '/':
            if (buf.y == 0)
                buf.code = -1; //代表除0
            else
                buf.result = buf.x / buf.y;
            break;
        default:
            buf.code = -2; //代表请求方法异常
            break;
        }

        buf.mtype = SERVER_TYPE;
        SendMsgQueue(msgid, buf);
        std::cout << "send done, code:" << buf.code << std::endl;
    }

    DestoryMsgQueue(msgid);

    return 0;
}
