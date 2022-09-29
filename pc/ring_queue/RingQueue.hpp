/*************************
    RingQueue类的实现
    利用信号量--环形队列存取任务
**************************/
#pragma once

#include <iostream>
#include <vector>
#include <pthread.h>
#include <semaphore.h>

namespace ns_ring_queue
{
    const int g_cap_default = 10;

    template <class T>
    class RingQueue
    {
    private:
        std::vector<T> ring_queue_;
        int cap_;
        sem_t blank_sem_;
        sem_t data_sem_;

        int c_step_;
        int p_step_;

        pthread_mutex_t c_mtx_;
        pthread_mutex_t p_mtx_;

    public:
        RingQueue(int cap = g_cap_default) : ring_queue_(cap), cap_(cap)
        {
            sem_init(&blank_sem_, 0, cap);
            sem_init(&data_sem_, 0, 0);
            c_step_ = p_step_ = 0;

            pthread_mutex_init(&c_mtx_, nullptr);
            pthread_mutex_init(&p_mtx_, nullptr);
        }
        ~RingQueue()
        {
            sem_destroy(&blank_sem_);
            sem_destroy(&data_sem_);

            pthread_mutex_destroy(&c_mtx_);
            pthread_mutex_destroy(&p_mtx_);
        }

    public:
        void Push(const T &in)
        {
            // P()
            sem_wait(&blank_sem_); 
            //理论上锁在wait前或后都是可以的，但在wait后可提高效率
            //因为此时线程都在等待队列，一旦有锁让出来就可以马上申请锁处理任务了。
            pthread_mutex_lock(&p_mtx_);
            //访问临界资源，添加任务
            ring_queue_[p_step_] = in;
            p_step_++;   
            p_step_ %= cap_;
            pthread_mutex_unlock(&p_mtx_);
            // V()
            sem_post(&data_sem_); 
        }
        void Pop(T *out)
        {
            //P()
            sem_wait(&data_sem_); 
            pthread_mutex_lock(&c_mtx_);
            *out = ring_queue_[c_step_];
            c_step_++;
            c_step_ %= cap_;
            pthread_mutex_unlock(&c_mtx_);
            //V()
            sem_post(&blank_sem_);
        }
    };
}
