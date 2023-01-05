/******************************************************
 * protocol.hpp--自定义传输协议
 * 定义数据格式
 * 使用Jsoncpp序列化-反序列化
 * 
 * 
******************************************************/


#pragma once

#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

using namespace std;

// 1.请求格式
struct request_t
{
    int x;     //运算符左边的数
    int y;     //运算符右边的数
    char op;   //运算符--"+-*/%"
}; 

// 2.响应格式
struct response_t
{
    int code;   // {0:success, -1:div 0, -2:mod 0, -3:op_wrong} 
    int result; // 计算结果
};

// 序列化--反序列化

//request_t -> string
std::string SerializeRequest(const request_t &req)
{
    Json::Value root; 
    root["datax"] = req.x;
    root["datay"] = req.y;
    root["operator"] = req.op;

    Json::FastWriter writer;
    std::string json_string = writer.write(root);
    return json_string;
}

// string -> request_t
void DeserializeRequest(const std::string &json_string, request_t &out)
{
    //反序列化
    Json::Reader reader;
    Json::Value root;

    reader.parse(json_string, root);
    out.x = root["datax"].asInt();
    out.y = root["datay"].asInt();
    out.op = (char)root["operator"].asInt();
}

// response_t->string
string SerializeResponse(const response_t &resp)
{
    Json::Value root;
    root["code"] = resp.code;
    root["result"] = resp.result;

    Json::FastWriter writer;
    string res = writer.write(root);

    return res;
}

// string->response_t
void DeserializeResponse(const string &json_string, response_t &out)
{
    Json::Reader reader;
    Json::Value root;

    reader.parse(json_string, root);
    out.code = root["code"].asInt();
    out.result = root["result"].asInt();
}




