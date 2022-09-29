/******************************************************
 * 懒汉单例线程池--在使用时再创建对象，创建进程
 * 使用静态成员函数 ThreadPool<Task>::GetInstance() 返回单例对象
 * 使用 PushTask()函数添加任务类的对象并执行
 * 所传入的任务类需要有operator()的重载才能执行对应重载任务
*******************************************************/



#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <unistd.h>
#include <pthread.h>

namespace ns_threadpool
{
    //默认最大线程数
    const int g_num = 5;

    template <class T>
    class ThreadPool
    {
    private:

        int num_;     //最大线程数
        std::queue<T> task_queue_;  //任务队列

        pthread_mutex_t mtx_;
        pthread_cond_t cond_;

        static ThreadPool<T> *ins; //静态对象

    private:
        // 构造函数必须得实现，但是必须要私有化
        ThreadPool(int num = g_num) : num_(num)
        {
            pthread_mutex_init(&mtx_, nullptr);
            pthread_cond_init(&cond_, nullptr);
        }
       
        //"=delete"的主要用途是阻止类的拷贝赋值
        //C++11表明这个函数是被删除的函数，虽然声明了它，但不能使用它
        ThreadPool(const ThreadPool<T> &tp) = delete;
        ThreadPool<T> &operator=(ThreadPool<T> &tp) = delete;

    public:
        //调用该静态成员函数来创建或返回单例对象
        static ThreadPool<T> *GetInstance()
        {
            //静态互斥锁
            static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
            // 判断是否是第一次创建单例对象
            if (ins == nullptr) //双判定，减少锁的争用，提高获取单例的效率！
            {
                pthread_mutex_lock(&lock);
                if (ins == nullptr)
                {
                    ins = new ThreadPool<T>();
                    ins->InitThreadPool();
                    std::cout << "首次加载对象" << std::endl;
                }
                pthread_mutex_unlock(&lock);
            }
            return ins;
        }

        void Lock(){pthread_mutex_lock(&mtx_);}
        void Unlock(){pthread_mutex_unlock(&mtx_);}
        void Wait(){pthread_cond_wait(&cond_, &mtx_);}
        void Wakeup(){pthread_cond_signal(&cond_);}
        bool IsEmpey(){return task_queue_.empty();}

        // 线程在类中需要执行静态函数(this指针)
        static void *Rountine(void *args)
        {
            //线程分离
            pthread_detach(pthread_self());
            ThreadPool<T> *tp = (ThreadPool<T> *)args;

            while (true)
            {
                //临界区，取出任务
                tp->Lock();
                while (tp->IsEmpey())
                {
                    tp->Wait();
                }
                //执行任务
                T t;
                tp->PopTask(&t);
                tp->Unlock();
                t();
            }
        }
        void InitThreadPool()
        {
            pthread_t tid;
            for (int i = 0; i < num_; i++)
            {
                pthread_create(&tid, nullptr, Rountine, (void *)this);
            }
        }
        void PushTask(const T &in)
        {
            //临界区--向任务队列添加任务
            Lock();
            task_queue_.push(in);
            Unlock();
            Wakeup();
        }
        //返回并弹出任务队列第一个任务
        void PopTask(T *out)
        {
            *out = task_queue_.front();
            task_queue_.pop();
        }
        ~ThreadPool()
        {
            pthread_mutex_destroy(&mtx_);
            pthread_cond_destroy(&cond_);
        }
    };

    //初始化静态成员
    template <class T>
    ThreadPool<T> *ThreadPool<T>::ins = nullptr;
}

