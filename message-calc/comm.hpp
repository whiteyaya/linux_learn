/*************************************************************
 * 消息队列：简易四则运算器
 * comm.hpp--协议文件
 * 支持 + = * /
 * 使用make命令编译程序、make clean命令清除可执行程序
 * 先执行server再执行client
 * client端按提示输入所需数据
 * 退出时client端输入"n"
*************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

// 消息结构
struct msg_buf
{
    long mtype;
    int source_pid;
    double x, y;
    char op;
    double result;
    int code;         //0:success, -1:div 0, -2:op_wrong, -999:quit
};

// 发送/接收数据的大小(不计算long mtype)
int msg_size = sizeof(struct msg_buf) - sizeof(long);

// 定义消息类型
#define SERVER_TYPE 1
#define CLIENT_TYPE 2

// 客户端读取数据
int ReadText(msg_buf& buf)
{
    char flag;
    std::cout<<"Ready to Start?(y/n)"<<std::endl;
    std::cin>>flag;
    if(flag=='n')
    {
        return -1;
    }
    std::cout<<"Please Enter X:"<<std::endl;
    std::cin>>buf.x;
    std::cout<<"Please Enter OP:"<<std::endl;
    std::cin>>buf.op;
    std::cout<<"Please Enter Y:"<<std::endl;
    std::cin>>buf.y;
    return 0;
}

// 创建/获取msg_id
int CommMsgQueue(int flags)
{
    key_t key = ftok("/tmp", 0x6666);
    // std::cout<<key<<std::endl;
    if(key < 0)
    {
        perror("ftok");
        return -1;
    }

    int msg_id = msgget(key, flags);
    if(msg_id < 0)
    {
        perror("msgget");
    }
    return msg_id;
}

int CreateMsgQueue()
{
    return CommMsgQueue(IPC_CREAT|IPC_EXCL|0666);
}

int GetMsgQueue()
{
    return CommMsgQueue(IPC_CREAT);
}

// 清除消息队列
int DestoryMsgQueue(int msg_id)
{
    if(msgctl(msg_id, IPC_RMID, NULL) < 0)
    {
        perror("msgctl");
        return -1;
    }
    return 0;
}

// 发送消息
int SendMsgQueue(int msg_id, msg_buf& buf)
{
    if(msgsnd(msg_id, (void*)&buf, msg_size, 0) < 0)
    {
        perror("msgsnd");
        return -1;
    }
    return 0;
}

// 接收消息
int RecvMsgQueue(int msg_id, msg_buf& out)
{
    if(msgrcv(msg_id, (void*)&out, msg_size, out.mtype, 0) < 0)
    {
        perror("msgrcv");
        return -1;
    }

    return 0;
}



