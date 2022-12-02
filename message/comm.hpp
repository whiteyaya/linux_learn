/*************************************************************
 * 消息队列：1-1进程间通信
 * 使用make命令编译程序
 * 先执行server再执行client
 * 退出时client端输入"quit"以清除消息队列
*************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

struct msg_buf
{
    long mtype;
    char mtext[1024];
};

#define SERVER_TYPE 1
#define CLIENT_TYPE 2

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

int DestoryMsgQueue(int msg_id)
{
    if(msgctl(msg_id, IPC_RMID, NULL) < 0)
    {
        perror("msgctl");
        return -1;
    }
    return 0;
}

int SendMsgQueue(int msg_id, int who, char* msg)
{
    struct msg_buf buf;
    buf.mtype = who;
    strcpy(buf.mtext, msg);

    if(msgsnd(msg_id, (void*)&buf, sizeof(buf.mtext), 0) < 0)
    {
        perror("msgsnd");
        return -1;
    }
    return 0;
}

int RecvMsgQueue(int msg_id, int recvType, char out[])
{
    struct msg_buf buf;
    int size=sizeof(buf.mtext);
    if(msgrcv(msg_id, (void*)&buf, size, recvType, 0) < 0)
    {
        perror("msgrcv");
        return -1;
    }

    strncpy(out, buf.mtext, size);
    out[size] = 0;
    return 0;
}



