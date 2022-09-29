#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <unistd.h>
#include <pthread.h>

namespace ns_threadpool
{
    const int g_num = 5;

    template <class T>
    class ThreadPool
    {
    private:
        int num_;
        std::queue<T> task_queue_; //该成员是一个临界资源

        pthread_mutex_t mtx_;
        pthread_cond_t cond_;

    public:
        void Lock()
        {
            pthread_mutex_lock(&mtx_);
        }
        void Unlock()
        {
            pthread_mutex_unlock(&mtx_);
        }
        void Wait()
        {
            pthread_cond_wait(&cond_, &mtx_);
        }
        void Wakeup()
        {
            pthread_cond_signal(&cond_);
        }
        bool IsEmpey()
        {
            return task_queue_.empty();
        }

    public:
        ThreadPool(int num = g_num) : num_(num)
        {
            pthread_mutex_init(&mtx_, nullptr);
            pthread_cond_init(&cond_, nullptr);
        }
        //必须让线程执行静态方法，非静态会自带参数this指针
        static void *Rountine(void *args)
        {
            pthread_detach(pthread_self());
            ThreadPool<T> *tp = (ThreadPool<T> *)args;

            while (true)
            {
                //临界区，判断任务队列
                //利用条件变量竞争任务
                tp->Lock();
                while (tp->IsEmpey())
                {
                    tp->Wait();
                }
                T t;
                //取出任务
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
                pthread_create(&tid, nullptr, Rountine, (void *)this);//将this指针传入
            }
        }
        void PushTask(const T &in)
        {
            Lock();
            task_queue_.push(in);
            Unlock();
            Wakeup();
        }
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
}

