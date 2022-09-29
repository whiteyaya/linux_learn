/***************************************************
    多线程环形队列实现多生产者--多消费者任务模型
***************************************************/


#include "RingQueue.hpp"
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "Task.hpp"

using namespace ns_ring_queue;
using namespace ns_task;

void *consumer(void *args)
{
    RingQueue<Task> *rq = (RingQueue<Task> *)args;
    while (true)
    {
        //取出后执行
        Task t;
        rq->Pop(&t);
        t();
        sleep(1);
    }
}

void *producter(void *args)
{
    //随机添加任务
    RingQueue<Task> *rq = (RingQueue<Task> *)args;
    const std::string ops = "+-*/%";
    while (true)
    {
        int x = rand() % 20 + 1;
        int y = rand() % 10 + 1;
        char op = ops[rand() % ops.size()];
        Task t(x, y, op);
        std::cout << "生产数据是:  " << t.Show() << "我是: " << pthread_self() << std::endl;
        rq->Push(t);
    }
}

int main()
{
    
    srand((long long)time(nullptr));
    RingQueue<Task> *rq = new RingQueue<Task>();

    //多生产者--多消费者
    pthread_t c[3];
    pthread_t p[3];
    for (int i = 0; i < 3; i++)
    {
        pthread_create(c + i, nullptr, consumer, (void *)rq);
        pthread_create(p + i, nullptr, producter, (void *)rq);
    }
    for (int i = 0; i < 3; i++)
    {
        pthread_join(c[i], nullptr);
        pthread_join(p[i], nullptr);
    }

    return 0;
}
