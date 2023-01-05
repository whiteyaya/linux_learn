/*************************************************************
 * 消息队列：简易四则运算器
 * client.cc--客户端源代码
 * 支持 + = * /
 * 端按提示输入所需数据
 * 退出时client端输入"n"
*************************************************************/

#include "comm.hpp"

int main()
{
    int msgid = GetMsgQueue();
    int pid = getpid();
    msg_buf buf;

    // 服务循环
    while(1)
    {
        buf.mtype = CLIENT_TYPE;

        // 读取用户输入
        if(ReadText(buf)==-1)
        {
            // 退出程序
            std::cout<<"Bye~"<<std::endl;
            buf.code = -999;
            SendMsgQueue(msgid, buf);
            break;
        }

        buf.source_pid = pid;
        SendMsgQueue(msgid, buf);
        printf("send done...\n");

        buf.mtype = SERVER_TYPE;
        RecvMsgQueue(msgid, buf);
        // 根据code输出
        if(buf.code==0)
        {
            std::cout<<"Server Answer:"<<buf.result<<std::endl;
        }
        else if(buf.code==-1)
        {
            std::cout<<"WRONG: can not div 0!"<<std::endl;
        }
        else
        {
            std::cout<<"WRONG: wrong op!"<<std::endl;
        }
    }

    return 0;
}
