#include "threadpool.h"

//初始化线程池，创建初始线程
template<typename T>
threadpool<T>::threadpool(int actor_model, connection_pool *connPool,int thread_number, int max_requests):
    m_actor_mode(actor_model),
    m_thread_number(thread_number),
    m_max_requests(max_requests),
    m_threads(NULL),
    m_connPool(connPool)
{
    if(thread_number <= 0 || max_requests <= 0)
    {
        throw std::exception();
    }

    m_threads = new pthread_t[m_thread_number];
    if(!m_threads) throw std::exception();
    
    //创建初始线程
    for(int i = 0; i < thread_number; i++)
    {
        if(pthread_create(m_threads + i, NULL, worker, this))
        {
            delete[] m_threads;//释放整个m_threads数组
            throw std::exception();
        }
        if(pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw std::exception();
        }
    }

}

template<typename T>
threadpool<T>::~threadpool()
{
    delete[] m_threads;
}

//添加任务
template<typename T>
bool threadpool<T>::append(T *request, int state)
{
    m_queuelocker.lock();//加锁
    //任务队列满了
    if(m_workqueue.size() >= m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }

    request->m_state = state;
    m_workqueue.push_back(request);//添加进任务队列

    m_queuelocker.unlock();//解锁
    m_queuestat.post();//信号量++

    return true;
}

//添加任务
template<typename T>
bool threadpool<T>::append_p(T *request)
{
    m_queuelocker.lock();
    //任务队列已满
    if(m_workqueue.size() >= m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();

    m_queuestat.post();//信号量++ 
    return true;
}

/*工作线程运行的函数，它不断从工作队列中取出任务并执行之*/
template<typename T>
void *threadpool<T>::worker(void *arg)
{
    threadpool *pool = (threadpool*) arg;
    pool->run();
    return pool;
}
//执行函数
template<typename T>
void threadpool<T>::run()
{
    while(true)
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if(m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }

        //取出任务，执行任务
        T *request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();

        if(!request)continue;

        if(m_actor_mode = 1)
        {
            //任务类型1
        }
        else
        {
            //任务类型2
        }


    }

}