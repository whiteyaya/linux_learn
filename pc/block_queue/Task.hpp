#pragma once

#include <iostream>
#include <pthread.h>

namespace ns_task
{
    class Task
    {
    private:
        int x_;
        int y_;
        char op_;

    public:
        Task() {}
        Task(int x, int y, char op) : x_(x), y_(y), op_(op) {}
        int Run()
        {
            int res = 0;
            switch (op_)
            {
            case '+':
                res = x_ + y_;
                break;
            case '-':
                res = x_ - y_;
                break;
            case '*':
                res = x_ * y_;
                break;
            case '/':
                res = x_ / y_;
                break;
            case '%':
                res = x_ % y_;
                break;
            default:
                std::cout << "Error!" << std::endl;
                break;
            }
            std::cout << "当前任务正在被: " << pthread_self() << " 处理: "
                      << x_ << op_ << y_ << "=" << res << std::endl;
            return res;
        }
        //重载()
        int operator()()
        {
            return Run();
        }
        ~Task() {}
    };
}
