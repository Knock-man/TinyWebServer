#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>

#include "http/http_conn.h"
#include "mysql/sql_connection_pool.h"

const int MAX_FD = 65536; //最大文件描述符
const int MAX_EVENT_NUMBER = 10000;//最大事件数
const int TIMESLOT = 5;//最小超时单位

class WebServer
{
public:
    WebServer();
    ~WebServer();

    

public:
    //基础
    int m_port;
    char *m_root;
    int m_log__write;
    int m_close_log;
    int m_actormodel;

    int m_pipefd[2];
    int m_epollfd;
    http_conn *users;

    //数据库相关
    connection_pool *m_connPool;
    std::string m_user;
    std::string m_passWord;
    std::string m_databaseName;
    int m_sql_num;

    //线程池相关
    threadpool<http_conn> *m_pool;
    int m_thread_num;

    //epoll_event相关
    epoll_event event[MAX_EVENT_NUMBER];

    int m_listenfd;
    int m_OPT_LINGER;
    int m_TRIGMode;
    int m_LISTENTrigmode;
    int m_CONNTrigmode;;

    //定时器相关
    
    
};



#endif