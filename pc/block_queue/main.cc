#include "BlockQueue.hpp"
#include "Task.hpp"
#include <time.h>
#include <cstdlib>
#include <unistd.h>
#define Num 5
using namespace ns_blockqueue;
using namespace ns_task;

//负责执行任务
void *consumer(void *args)
{
    BlockQueue<Task> *bq = (BlockQueue<Task> *)args;
    while (true)
    {
        Task t;
        bq->Pop(&t); //取出任务
        t();         //执行任务
    }
}

//负责配发任务
void *producter(void *args)
{
    //生成随机任务
    BlockQueue<Task> *bq = (BlockQueue<Task> *)args;
    std::string ops = "+-*/%";
    while (true)
    {
        int x = rand() % 20 + 1; //[1,20]
        int y = rand() % 10 + 1; //[1,10]
        char op = ops[rand() % 5];
        Task t(x, y, op);
        std::cout << "生产者派发了一个任务: " << x << op << y << "=?" << std::endl;
        // 将任务加入任务队列
        bq->Push(t);
        sleep(1);
    }
}

int main()
{
    srand((long long)time(nullptr));
    BlockQueue<Task> *bq = new BlockQueue<Task>();
    pthread_t p, c[Num];
    //一个生产者--多个消费者
    pthread_create(&p, nullptr, producter, (void *)bq);
    for (int i = 0; i < Num; i++)
    {
        pthread_create(c + i, nullptr, consumer, (void *)bq);
    }

    pthread_join(p, nullptr);
    for (int i = 0; i < Num; i++)
    {
        pthread_join(c[i], nullptr);
    }

    return 0;
}
