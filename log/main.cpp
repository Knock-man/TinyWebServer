#include<sys/time.h>
#include<iostream>
#include"block_queue.h"
#include<stdio.h>
#include<thread>
#include<chrono>

void t1(block_queue<int> *queue)
{
    for(int i=0;i<10;i++)
    {
        std::cout<<"t1 插入 10" <<std::endl;
        queue->push(10);
    }

    for(int i=0;i<5;i++)
    {
        int tem;
        queue->pop(tem);
        std::cout<<"取出"<<tem<<std::endl;
    }
}

void t2(block_queue<int> *queue)
{
    for(int i=0;i<10;i++)
    {
        std::cout<<"t2 插入 20" <<std::endl;
        queue->push(20);
    }

    for(int i=0;i<5;i++)
    {
        int tem;
        queue->pop(tem);
        std::cout<<"取出"<<tem<<std::endl;
    }
}

void t3(block_queue<int> *queue)
{
    for(int i=0;i<10;i++)
    {
        std::cout<<"t2 插入 30" <<std::endl;
        queue->push(30);
    }

    for(int i=0;i<5;i++)
    {
        int tem;
        queue->pop(tem);
        std::cout<<"取出"<<tem<<std::endl;
    }
}
int main()
{

    // struct timeval now = {0, 0};
    // gettimeofday(&now, NULL);
    // std::cout<<now.tv_sec<<" "<<now.tv_usec<<std::endl;

    block_queue<int> queue(20);
    std::thread a(t1,&queue);
    std::thread b(t2,&queue);
    std::thread c(t3,&queue);

    std::this_thread::sleep_for(std::chrono::seconds(10));
    

    


}