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

        //静态单例对象
        static ThreadPool<T> *ins; 

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
        //创建或返回静态单例对象ins
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
        //互斥锁上锁
        void Lock(){pthread_mutex_lock(&mtx_);}
        //互斥锁解锁
        void Unlock(){pthread_mutex_unlock(&mtx_);}
        //将该线程挂起，加入等待队列，暂时释放互斥锁
        void Wait(){pthread_cond_wait(&cond_, &mtx_);}
        //在等待队列中唤醒一个线程
        void Wakeup(){pthread_cond_signal(&cond_);}
        bool IsEmpey(){return task_queue_.empty();}

        // 多线程在类中需要执行静态函数(this指针)
        // 执行任务队列中的任务
        static void *Rountine(void *args)
        {
            //线程分离
            pthread_detach(pthread_self());
            ThreadPool<T> *tp = (ThreadPool<T> *)args;

            while (true)
            {
                //临界区，线程间互斥取出任务
                tp->Lock();
                while (tp->IsEmpey())
                {
                    tp->Wait();
                }
                //无任务所有线程都会进入等待队列，唤醒时再竞争互斥锁
                //执行任务
                T t;
                tp->PopTask(&t);
                tp->Unlock();
                t();
            }
        }
        //初始化线程池，创建线程
        void InitThreadPool()
        {
            pthread_t tid;
            for (int i = 0; i < num_; i++)
            {
                pthread_create(&tid, nullptr, Rountine, (void *)this);
            }
        }
        //将任务加入任务队列
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

    //初始化未被调用时的静态成员为nullpty
    template <class T>
    ThreadPool<T> *ThreadPool<T>::ins = nullptr;
}

