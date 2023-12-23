#include "log.h"

#include <stdarg.h>


Log::Log()
{
    m_count = 0;
    m_is_async = false;
}

Log::~Log()
{
    if(m_fp != NULL)
    {
        fclose(m_fp);
    }
}

//将日志写入文件
void *Log::async_write_log()
{
    std::string single_log;
    //从阻塞队列中取出一个日志string, 写入文件
    while(m_log_queue->pop(single_log))
    {
        m_mutex.lock();
        fputs(single_log.c_str(),m_fp);
        m_mutex.unlock();
    }

}

Log *Log::get_instance()
{
    static Log instance;
    return &instance;
}

void *Log::flush_log_thread(void *args)
{
    Log::get_instance()->async_write_log();
}

//异步需要设置阻塞队列长度，同步不需要
bool Log::init(const char *file_name, int close_log, int log_buf_size, int split_lines , int max_queue_size)
{
    //如果设置了max_queue_size，则设置为异步
    if(max_queue_size >= 1)
    {
        m_is_async = true;//异步
        m_log_queue = new block_queue<std::string>(max_queue_size);
        pthread_t tid;
        //flush_log_thread为回调函数,这里表示创建线程异步写日志
        //int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine)(void*), void *arg);
        pthread_create(&tid,NULL,flush_log_thread,NULL);
    }

    m_close_log = close_log;
    m_log_buf_size = log_buf_size;

    m_buf = new char[m_log_buf_size];
    memset(m_buf,'\0',m_log_buf_size);

    m_split_lines = split_lines;

    time_t t = time(NULL);//获取当前时间
    struct tm * my_tm = localtime(&t);//转化为本地时间
    //struct tm my_tm = *sys_tm;

    //strrchr搜索字符串中最后一次出现某个字符的位置,没有找到返回空
    const char *p = strrchr(file_name,'/');
    char log_full_name[256] = {0};

    if(p == NULL)//没有'/' 给的就是文件名
    {
        snprintf(log_full_name,255,"%d_%02d_%02d_%s",my_tm->tm_year + 1900, my_tm->tm_mon + 1, my_tm->tm_mday, file_name);
    }
    else //目录+文件名
    {
        strcpy(log_name, p + 1);//strcpy(char*) //取出文件名
        strncpy(dir_name,file_name,p-file_name+1);//取出目录
        snprintf(log_full_name,255,"%s%d_%02d_%02d_%s",dir_name,my_tm->tm_year + 1900, my_tm->tm_mon + 1, my_tm->tm_mday, log_name);
    }

    m_today = my_tm->tm_mday;
    m_fp = fopen(log_full_name,"a");//打开日志文件
    if(m_fp == NULL) return false;
    return true;

}

//写日志
void Log::write_log(int level, const char *format, ...)
{
    struct timeval now = {0,0};
    gettimeofday(&now, NULL);
    time_t t = now.tv_sec;
    struct tm *my_tm = localtime(&t);
    char s[26] = {0};
    
    //严重程度
    switch(level)
    {
        case 0:
            strcpy(s,"[debug]:");
            break;
        case 1:
            strcpy(s,"[info]:");
            break;
        case 2:
            strcpy(s,"[warn]:");
            break;
        case 3:
            strcpy(s,"[errno]");
            break;
        default:
            strcpy(s,"[info]:");
            break;
    }

    //写入一个log,对m_count++,m_split_lines最大行数
    m_mutex.lock();
    m_count++;

    if(m_today != my_tm->tm_mday || m_count % m_split_lines == 0)//日期不同，超行
    {
        char new_log[256] = {0};
        fflush(m_fp);//将数据从程序的内存缓冲区刷新到文件中
        fclose(m_fp);
        char tail[16] = {0};

        snprintf(tail,16,"%d_%02d_%02d_",my_tm->tm_yday + 1900, my_tm->tm_mon + 1, my_tm->tm_mday);

        if(m_today != my_tm->tm_mday)//超天
        {
            snprintf(new_log,255,"%s%s%s",dir_name,tail,log_name);
            m_today = my_tm->tm_mday;
            m_count = 0;
        }
        else//超行
        {
            snprintf(new_log, 255, "%s%s%s.%lld", dir_name, tail, log_name, m_count / m_split_lines);
        }
        m_fp = fopen(new_log,"a");

        m_mutex.unlock();
        //可变参
        va_list valst;
        va_start(valst, format);

        std::string log_str;
        m_mutex.lock();

         //写入的具体时间内容格式
        int n = snprintf(m_buf, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld %s ",
                     my_tm->tm_year + 1900, my_tm->tm_mon + 1, my_tm->tm_mday,
                     my_tm->tm_hour, my_tm->tm_min, my_tm->tm_sec, now.tv_usec, s);
        //变参写入写入提示内容
        int m = vsnprintf(m_buf + n,m_log_buf_size-n-1,format,valst);
        m_buf[n+m] = '\n';
        m_buf[n+m+1] = '\0';
        log_str = m_buf;

        m_mutex.unlock();

        //写入文件
        if(m_is_async && !m_log_queue->full())//为异步日志 且 日志阻塞队列没有满
        {
            m_log_queue->push(log_str);
        }
        else//为同步日志
        {
            m_mutex.lock();
            fputs(log_str.c_str(),m_fp);
            m_mutex.unlock();
        }

        va_end(valst);
        
    }

}

void Log::flush(void)
{
    m_mutex.lock();
    //强制写入缓冲区
    fflush(m_fp);
    m_mutex.unlock();
}