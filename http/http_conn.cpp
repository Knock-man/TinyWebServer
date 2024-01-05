#include "http_conn.h"

#include <fcntl.h>
#include <unistd.h>


//设置文件描述符非阻塞
int setnonblocking(int fd)
{
    int old_option = fcntl(fd,F_GETFL);//获取原始描述符状态标志
    int new_option = old_option | O_NONBLOCK;//添加非阻塞标志
    fcntl(fd,F_SETFL,new_option);//将新的状态标志应用到文件描述符
    return old_option;
}

//epoll操作

//内核表注册新事件
void addfd(int epollfd, int fd, bool one_shot, int TRIGMode)
{
    epoll_event event;
    event.data.fd = fd;

    if(TRIGMode == 1)//监听读事件,ET模式，描述符挂断
    {
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    }
    else//监听读事件，LT模式，描述符挂断
    {
        event.events = EPOLLIN | EPOLLRDHUP;
    }

    if(one_shot)//只监听一次
    {
        event.events |= EPOLLONESHOT;//只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里
    }

    //描述符挂上红黑树
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);

    //设置非阻塞
    setnonblocking(fd);
}

//内核表删除事件
void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
    close(fd);
}

//将事件重置为EPOLLONESHOT
void modfd(int epollfd, int fd, int ev, int TRIGMode)
{
    epoll_event event;
    event.data.fd = fd;
    
    if(TRIGMode == 1)//ET模式
    {
        event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    }
    else//LT模式
    {
        event.events = ev | EPOLLONESHOT | EPOLLRDHUP;
    }

    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);
}

int http_conn::m_user_count = 0;
int http_conn::m_epollfd = -1;

//初始化套接字地址，函数内部会调用私有方法init
void http_conn::init(int sockfd, const sockaddr_in &addr,char *root, int TRIGMode, int close_log, std::string user, std::string passwd, std::string sqlname)
{
    m_sockfd = sockfd;
    m_address = addr;
    
    addfd(m_epollfd,sockfd,true,m_TRIGMode);
    m_user_count++;

    //当浏览器出现连接重置时，可能是网站根目录出错或http响应格式出错或者访问的文件内容完全为空
    doc_root = root;
    m_TRIGMode = TRIGMode;//ET(1) LT(0)
    m_close_log = close_log;//日志

    strcpy(sql_user,user.c_str());
    strcpy(sql_passwd,passwd.c_str());
    strcpy(sql_name, sqlname.c_str());

    init();

}

//关闭http连接
void http_conn::close_conn(bool real_close)
{

}

void http_conn::process()
{

}

//读取浏览器端发来的全部数据
bool http_conn::read_once()
{
    if(m_read_idx >= READ_BUFFER_SIZE)//读缓冲区已满
    {
        return false;
    }
    int bytes_read = 0;

    //LT读取数据
    if(m_TRIGMode == 0)
    {
        //从m_read_buf空闲区开始读
        bytes_read = recv(m_sockfd,m_read_buf+m_read_idx,READ_BUFFER_SIZE-m_read_idx,0);
        m_read_idx += bytes_read;//?为什么不放在下一行判断代码之后

        if(bytes_read <= 0)
        {
            return false;//读取失败
        }

        return true;
    }
    //ET读数据
    else
    {
        while(true)
        {
            bytes_read = recv(m_sockfd,m_read_buf+m_read_idx,READ_BUFFER_SIZE-m_read_idx,0);
            if(bytes_read == -1)//发生，检查errno
            {
                if(errno == EAGAIN || errno == EWOULDBLOCK)//系统资源暂时不可用，需要重试操作//?为什么break之后可以重试操作，break出去岂不是要等到有新事件发生才能触发读取
                {
                    break;//重新监听
                } 
                return false;//其他错误,停止监听
            }
            else if(bytes_read == 0)//对端关闭,停止监听
            {
                return false;
            }
            //正常读取
            m_read_idx += bytes_read;
        }
        return true;
    }
}

//响应报文写入函数
bool http_conn::write()
{

}


//同步线程初始化数据库读取表
void http_conn::initmysql_result(connection_pool *connPool)
{

}
//初始化新接受的连接
//check_state默认为分析请求行状态
void http_conn::init()
{
    mysql = NULL;
    bytes_to_send = 0;//剩余发送字节数
    bytes_have_send = 0;//已发送字节数
    m_check_state = CHECK_STATE_REQUESTLINE;//主状态机状态
    m_linger = false;
    m_method = GET;
    m_url = 0;
    m_version = 0;
    m_content_length = 0;
    m_host = 0;
    m_start_line = 0;
    m_checked_idx = 0;
    m_read_idx = 0;
    m_write_idx = 0;
    cgi = 0;
    m_state = 0;
    timer_flag = 0;
    improv = 0;

    memset(m_read_buf,'\0',READ_BUFFER_SIZE);
    memset(m_write_buf,'\0',WRITE_BUFFER_SIZE);
    memset(m_real_file,'\0',FILENAME_LEN);


}
//从m_read_buf读取，并处理请求报文
http_conn::HTTP_CODE http_conn::process_read()
{

}
//向m_write_buf写入响应报文数据
bool http_conn::process_write(HTTP_CODE ret)
{

}
//主状态机解析报文中的请求行数据
http_conn::HTTP_CODE http_conn::parse_request_line(char *text)
{

}
//主状态机解析报文的请求头数据
http_conn::HTTP_CODE http_conn::parse_headers(char *text)
{

}
//主状态机解析报文中的请求内容
http_conn::HTTP_CODE http_conn::parse_content(char *text)
{

}
//生成响应报文
http_conn::HTTP_CODE http_conn::do_request()
{

}


//从状态机读取一行，分析是请求报文的哪一部分
http_conn::LINE_STATUS http_conn::parse_line()
{

}

void http_conn::umap()
{

}

//根据响应报文格式，生成对应8个部分，以下函数均又do_request调用
bool http_conn::add_response(const char *format, ...)
{

}
bool http_conn::add_content(const char *content)
{

}
bool http_conn::add_status_line(int status, const char *title)
{

}
bool http_conn::add_headers(int content_length)
{

}
bool http_conn::add_content_type()
{

}
bool http_conn::add_content_length(int content_length)
{

}
bool http_conn::add_linger()
{

}
bool http_conn::add_blank_line()
{

}

