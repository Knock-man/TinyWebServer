#ifndef LOG_H
#define LOG_H

#define LOG_DEBUG(format, ...) if( 0 == m_close_log ) {Log::get_instance()->write_log(0,format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_INFO(format, ...) if( 0 == m_close_log ) {Log::get_instance()->write_log(1,format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_WARN(format, ...) if( 0 == m_close_log ) {Log::get_instance()->write_log(2,format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_ERROR(format, ...) if( 0 == m_close_log ) {Log::get_instance()->write_log(3,format, ##__VA_ARGS__); Log::get_instance()->flush();}

#include <stdio.h>
#include <string.h>

#include "block_queue.h"
#include "../lock/locker.h"

class Log
{
public:
    static Log *get_instance();//单例模式

    static void *flush_log_thread(void *args);//刷新日志文件

    bool init(const char *file_name, int close_log, int log_buf_size = 8192, int split_lines = 5000000, int max_queue_size = 0);

    void write_log(int level, const char *format, ...);//写日志

    void flush(void);
   
private:
    Log();
    virtual ~Log();

    void *async_write_log();

private:
    char dir_name[128]; //log路径名
    char log_name[128]; //log文件名
    int m_split_lines; //日志最大行数
    int m_log_buf_size; //日志缓冲区大小
    long long m_count; //日志行数记录
    int m_today; //按天分类，记录当前时间是哪一天
    FILE *m_fp; //打开log的文件指针
    char *m_buf;//日志缓冲区
    block_queue<std::string> *m_log_queue;//日志队列
    bool m_is_async; //是否同步标志位
    locker m_mutex;
    int m_close_log; //关闭日志
    

};

#endif