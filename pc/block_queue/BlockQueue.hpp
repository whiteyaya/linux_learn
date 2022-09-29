//.hpp -> 开源软件使用 -> 声明和实现可以放在一个文件里
#pragma once
#include <iostream>
#include <queue>
#include <pthread.h>
#include <unistd.h>
/*
    const& 输入
    *      输出
    &      输入输出
*/
namespace ns_blockqueue
{
    const int default_cap = 5;//默认大小

    template <class T>
    class BlockQueue
    {
    private:
        std::queue<T> bq_;    //阻塞队列
        int cap_;             //队列的元素上限
        pthread_mutex_t mtx_; //保护临界资源的锁
        // 1. 当生产满了的时候，就应该不要生产了(不要竞争锁了)，而应该让消费者来消费
        // 2. 当消费空了，就不应该消费（不要竞争锁了）,应该让生产者来进行生产
        pthread_cond_t is_full_; //队列是否满的条件变量
        pthread_cond_t is_empty_; //队列是否为空的条件变量

        bool IsFull(){return bq_.size() == cap_;}
        bool IsEmpty(){return bq_.size() == 0;}
        void LockQueue(){pthread_mutex_lock(&mtx_);}
        void UnlockQueue(){pthread_mutex_unlock(&mtx_);}
        void ProducterWait(){
            // 1. 调用的时候，会首先自动释放mtx_,然后再挂起自己
            // 2. 返回的时候，会首先自动竞争锁，获取到锁之后，才能返回
            pthread_cond_wait(&is_empty_, &mtx_);
        }
        void ConsumerWait(){pthread_cond_wait(&is_full_, &mtx_);}
        void WakeupComsumer(){pthread_cond_signal(&is_full_);}
        void WakeupProducter(){pthread_cond_signal(&is_empty_);}

    public:
        BlockQueue(int cap = default_cap) : cap_(cap)
        {
            pthread_mutex_init(&mtx_, nullptr);
            pthread_cond_init(&is_empty_, nullptr);
            pthread_cond_init(&is_full_, nullptr);
        }
        ~BlockQueue()
        {
            pthread_mutex_destroy(&mtx_);
            pthread_cond_destroy(&is_empty_);
            pthread_cond_destroy(&is_full_);
        }

        void Push(const T &in)
        {
            //临界区
            LockQueue();
            //使用while不用if可以防止没有等待就去执行下面的push了
            //while没有在一直判断，因为线程被挂起了
            //会等待条件变量bond的解除信号来继续执行
            while (IsFull())
            {
                ProducterWait();
            }
            //向队列中放任务
            bq_.push(in);

            WakeupComsumer();
            UnlockQueue();
        }

        void Pop(T *out)
        {
            //临界区
            LockQueue();
            //使用循环原因同上
            while (IsEmpty())
            { 
                ConsumerWait();
            }
            //输出型参数out
            *out = bq_.front();
            bq_.pop();
            WakeupProducter();
            UnlockQueue();
            
        }
    };
}

