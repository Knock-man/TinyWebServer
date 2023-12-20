/*************************************************************
*循环数组实现的阻塞队列，m_back = (m_back + 1) % m_max_size;  
*线程安全，每个操作前都要先加互斥锁，操作完后，再解锁
**************************************************************/

#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <iostream>
#include <sys/time.h>

#include "../lock/locker.h"

template<class T>
class block_queue
{
public:
    block_queue(int max_size = 1000):
        m_max_size(max_size),
        m_size(0),
        m_front(-1),
        m_back(-1)

    {
        m_array = new T[max_size];
        if(max_size <= 0)
        {
            exit(-1);
        }
    }
    ~block_queue()
    {
        m_mutex.lock();
        if(m_array != NULL)
        {
            delete [] m_array;
        }
        m_mutex.unlock();
    }
    //清空队列
    void claer()
    {
        m_mutex.lock();
        m_size = 0;
        m_front = -1;
        m_back = -1;
        m_mutex.unlock();
    }
    //判断队列是否满了
    bool full()
    {
        m_mutex.lock();
        if(m_size >= m_max_size)
        {
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
        return false;
    }
    //判断队列是否为空
    bool empty()
    {
        m_mutex.lock();
        if(m_size <= 0)
        {
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
        return false;
    }
    //返回队首元素
    bool front(T &value)
    {
        m_mutex.lock();
        if(m_size <=0 )
        {
            m_mutex.unlock();
            return false;
        }
        value = m_array[m_front];
        m_mutex.unlock();
        return true;
    }
    //返回队尾元素
    bool back(T &value)
    {
        m_mutex.lock();
        if(m_size <= 0)
        {
            m_mutex.unlock();
            return false;
        }
        value = m_array[m_back];
        m_mutex.unlock();
        return true;
    }
    //获取队列元素个数
    int size()
    {
        int tmp = 0;
        m_mutex.lock();
        tmp = m_size;
        m_mutex.unlock();
        return tmp;
    }
    //获取队列大小
    int maxsize()
    {
        int tmp = 0;
        m_mutex.lock();
        tmp = m_max_size;
        m_mutex.unlock();
        return tmp;
    }
    //往队列添加元素，需要将所有使用队列的线程先唤醒
    //当有元素push进队列,相当于生产者生产了一个元素
    //若当前没有线程等待条件变量,则唤醒无意义
    bool push(const T &item)
    {
        m_mutex.lock();
        if(m_size >= m_max_size)
        {
            m_cond.broadcast();
            m_mutex.unlock();
            return false;
        }
        m_back = (m_back + 1) % m_max_size;
        m_array[m_back] = item;
        m_size++;

        m_cond.broadcast();
        m_mutex.unlock();
        return true;
    }
    //pop时,如果当前队列没有元素,将会等待条件变量
    bool pop(T &item)
    {
        m_mutex.lock();
        while(m_size <= 0)//阻塞条件
        {
            if(!m_cond.wait(m_mutex.get()))
            {
                //阻塞出错
                m_mutex.unlock();
                return false;
            }
        }
        //正常唤醒
        //出队
        m_front = (m_front + 1) % m_max_size;
        item = m_array[m_front];
        m_size--;
        m_mutex.unlock();
        return true;
    }
    //超时出队
    bool pop(T &item, int ms_timeout)//ms_timeout为毫秒
    {
        /*
        #include <sys/time.h>
        int gettimeofday(struct timeval* tv, struct timezone* tz);//获取当前时间,第二个参数通常传NULL(忽略时区)

        struct timeval {
            time_t      tv_sec;     // seconds 表示自 1970 年 1月1日以来的秒数
            suseconds_t tv_usec;    // microseconds 表示微秒数 
        };

        struct timespec {
            time_t tv_sec;  // seconds 表示自 1970 年 1月1日以来的秒数
            long tv_nsec;   // nanoseconds 表示纳秒数
        };
        */
        struct timespec t = {0, 0};//传入pthread_mutex_t *m_mutex,struct timespec t的结构体
        struct timeval now = {0, 0};//存储当前时间
        gettimeofday(&now,NULL);//获取当前时间

        m_mutex.lock();
        if(m_size <= 0)
        {
            /*
                1 秒 = 1000 毫秒（ms）
                1 毫秒 = 1000000 纳秒（ns）
            */
            t.tv_sec = now.tv_sec + ms_timeout / 1000; //计算超时秒数 ms_timeout / 1000 ：将毫秒转化为秒
            t.tv_nsec = (ms_timeout % 1000) * 1000; //ms_timeout % 1000取剩余秒余数部分(毫秒)   * 1000 将毫秒转为纳秒
            if(!m_cond.timewait(m_mutex.get(),t))//错误返回非零 超时返回-1
            {
                m_mutex.unlock();
                return false;
            }
        }
        //等待超时
        if(m_size <= 0)
        {
            m_mutex.unlock();
            return false;
        }

        //出队
        m_front = (m_front + 1) % m_max_size;
        item = m_array[m_front];
        m_size--;
        m_mutex.unlock();
        return true;
    }

private:
    locker m_mutex;
    cond m_cond;

    T *m_array;//循环阻塞队列
    int m_size;//队列大小
    int m_max_size;//队列容量
    int m_front;//队头指针
    int m_back;//队尾指针
};
#endif