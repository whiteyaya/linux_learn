/******************************************************
 * 懒汉单例线程池--SigThreadPool.hpp
 * 在使用时才创建对象，创建进程
 * 使用静态成员函数 ThreadPool<Task>::GetInstance() 返回单例对象
 * 使用 PushTask()函数添加任务类的对象并执行
 * 所传入的任务类需要有operator()的重载才能执行对应任务
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

        pthread_mutex_t mtx_; //互斥锁
        pthread_cond_t cond_; //条件变量

        static ThreadPool<T> *ins; //静态对象

    private:
        // 私有化构造函数
        ThreadPool(int num = g_num) : num_(num)
        {
            pthread_mutex_init(&mtx_, nullptr);
            pthread_cond_init(&cond_, nullptr);
        }
       
        // =delete--C++11表明这个函数是被删除的函数，虽然声明了它，但不能使用它
        ThreadPool(const ThreadPool<T> &tp) = delete;
        ThreadPool<T> &operator=(ThreadPool<T> &tp) = delete;

    public:
        //调用该静态成员函数来创建或返回单例对象
        static ThreadPool<T> *GetInstance()
        {
            //静态互斥锁--防止并发调用，多次创建对象
            static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

            // 判断是否是第一次创建单例对象
            // 双判定，减少锁的争用，提高获取单例的效率！
            if (ins == nullptr) 
            {
                pthread_mutex_lock(&lock);
                if (ins == nullptr)
                {
                    //创建单例对象
                    ins = new ThreadPool<T>();
                    ins->InitThreadPool();
                    //std::cout << "首次加载对象" << std::endl;
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

        // 执行任务队列中的任务
        // 多线程在类中需要执行静态函数(this指针)
        static void *Rountine(void *args)
        {
            //线程分离
            pthread_detach(pthread_self());
            //tp指向的是同一个单例对象
            ThreadPool<T> *tp = (ThreadPool<T> *)args;

            while (true)
            {
                //临界区，线程间互斥取出任务
                tp->Lock();
                //无任务所有线程都会进入等待队列，唤醒时再竞争互斥锁
                while (tp->IsEmpey())//不会一直循环，还能防止Wait()失败
                {
                    tp->Wait();
                }
                //本线程得到任务并执行
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

    //初始化静态成员--用来判断单例
    template <class T>
    ThreadPool<T> *ThreadPool<T>::ins = nullptr;
}

