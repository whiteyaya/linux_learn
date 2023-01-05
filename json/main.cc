/*********************************************
 * 用于简单学习Json序列化反序列化过程
 * 使用前请yum install -y jsoncpp-devel 编译时加 -ljsoncpp
 * 
*********************************************/

#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

// 数据结构
struct request_t
{
    int x;  
    int y;  
    char op;   
}; 

int main()
{
    // //==============序列化的过程===================//
    // 
    // request_t req = {10, 20, '*'};
    // Json::Value root; //Json::Value类似map，但是没有数据存储限制
    // // 添加数据
    // root["datax"] = req.x;
    // root["datay"] = req.y;
    // root["operator"] = req.op;

    // // StyledWriter格式：
    // // {
    // //     "datax" : 10,
    // //     "datay" : 20,
    // //     "operator" : 42
    // // }
    // // Json::StyledWriter writer;

    // // FastWriter格式：{"datax":10,"datay":20,"operator":42}
    // Json::FastWriter writer;

    // // 输出序列化结果
    // std::string json_string = writer.write(root);
    // std::cout << json_string << std::endl;


    // // ==============反序列化的过程===================//
    // // 自定义获取序列化数据--c++11利用R"()"将内容定义为纯字符串
    // std::string json_string = R"({"datax":10,"datay":20,"operator":42})";

    // Json::Reader reader;
    // Json::Value root;
    // // 进行反序列化json_string->root
    // reader.parse(json_string, root);

    // // 读取数据
    // request_t req;
    // req.x = root["datax"].asInt();
    // req.y = root["datay"].asInt();
    // req.op = (char)root["operator"].asInt();

    // std::cout << req.x << " " << req.op << " " << req.y << std::endl;   


    return 0;
}