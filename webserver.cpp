#include "webserver.h"

WebServer::WebServer()
{

}
WebServer::~WebServer()
{

}

void WebServer::init(int port, std::string user, std::string passWord,std::string databaseName,
            int log_write, int opt_linger, int trigmode, int sql_num,
            int thread_num, int close_log, int actor_model)
            {

            }
    
void WebServer::thread_pool()
{

}
void WebServer::sql_pool()
{

}
void WebServer::log_write()
{

}
void WebServer::trig_mode()
{

}
void WebServer::eventListen()
{
    //网络编程基本步骤
    m_listenfd = socket(PF_INET,SOCK_STREAM,0);
    assert(m_listenfd >= 0);

    //优雅关闭连接
    if(m_OPT_LINGER == 0)
    {
        struct linger tmp = {0,1};
        setsockopt(m_listenfd,SOL_SOCKET,SO_LINGER,&tmp,sizeof(tmp));
    }else if( m_OPT_LINGER == 1)
    {
        struct linger tmp = {1,1};//{启用SO_LINGER,等待时间1秒}
        setsockopt(m_listenfd,SOL_SOCKET,SO_LINGER,&tmp,sizeof(&tmp));
    }

    
    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(m_port);

    //设置端口复用
    int flag = 1;
    setsockopt(m_listenfd,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(flag));

    int ret = 0;
    ret = bind(m_listenfd,(struct sockaddr *)&address,sizeof(address));
    assert(ret >= 0);

    ret = listen(m_listenfd,5);
    assert(ret >= 0);

    //?时间初始化函数 utils.init(TIMESLOT);

    //epoll创建内核事件表
    epoll_event events[MAX_EVENT_NUMBER];
    m_epollfd = epoll_create(5);
    assert(m_epollfd != -1);

    //?时间类utils.addfd

    http_conn::m_epollfd = m_epollfd;

    //?时间类处理



}
void WebServer::eventLoop()
{
    bool timeout = false;
    bool stop_server = false;

    while(!stop_server)
    {
        int number = epoll_wait(m_epollfd,events,MAX_EVENT_NUMBER,-1);
        if(number < 0 && errno != EINTR)
        {
            LOG_ERROR("%s", "epoll failure");
            break;
        }

        for(int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;
            //处理新到的客户端连接
            if(sockfd == m_listenfd)
            {
                bool flag = dealclinetdata();
                if(flag == false) continue;//连接失败继续监听
            }
            //处理异常事件
            else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {

            }
            //处理信号
            else if((sockfd == m_pipefd[0]) && (events[i].events & EPOLLIN))
            {

            }
            //处理客户端连接上接收到的数据
            else if(events[i].events & EPOLLIN)
            {
                //读事件
                dealwithread(sockfd);
            }
            //处理分送至客户端数据
            else if(events[i].events & EPOLLOUT)
            {
                //写事件
                dealwithwrite(sockfd);
            }
            //超时
            if(timeout)
            {
                //?

            }
        }
    }

}
void WebServer::timer(int connfd, struct sockaddr_in client_address)
{

}
void WebServer::adjust_timer(util_timer *timer)
{

}
void WebServer::deal_timer(util_timer *timer, int sockfd)
{

}
bool WebServer::dealclinetdata()
{
    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);
    //LT模式
    if(m_LISTENTrigmode == 0)
    {
        int connfd = accept(m_listenfd, (struct sockaddr *)&client_address,&client_addrlength);
        //连接失败
        if(connfd < 0)
        {
            LOG_ERROR("%s:errno is:%d," "accept error",errno);
            return false;
        }
        //连接客户端过多
        if(http_conn::m_user_count >= MAX_FD)
        {
            //?utils.show_error
            LOG_ERROR("%s","Internal server busy");
            return false;
        }
        //? timer(connfd,client_address);
    }
    //ET模式
    else
    {
        while(1)
        {
            int connfd = accept(m_listenfd,(struct sockaddr *)&client_address, &client_addrlength);
            if(connfd < 0)
            {
                LOG_ERROR("%s:errno is:%d","accept error,",errno);
                break;
            }
            if(http_conn::m_user_count >= MAX_FD)
            {
                //?utils.show_error
                LOG_ERROR("%s","Internal server busy");
                break;
            }
            //?timer(connfd,client_address);
        }
        return false;
    }
    return true;

}
void WebServer::dealwithsignal(bool &timeout, bool &stop_server)
{

}
void WebServer::dealwithread(int sockfd)
{

}
void WebServer::dealwithwrite(int sockfd)
{

}