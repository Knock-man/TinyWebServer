#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include <iostream>
#include <list>
#include <mysql/mysql.h>

#include "../log/log.h"
#include "../lock/locker.h"

//连接池
class connection_pool
{
public:
    MYSQL *GetConnection(); //获取数据库连接
    bool ReleaseConnection(MYSQL *conn); //归还当前连接
    int GetFreeConn();  //获取当前空闲连接数量
    void DestroyPoll(); //销毁所有连接

    //单例模式
    static connection_pool *GetInstance();//获取连接池接口

    //初始化连接池
    void init(std::string url ,std::string User, std::string Password,std::string DBName, uint16_t Port = 3306, int MaxConn = 10, int close_log = false);

private:
    connection_pool();
    ~connection_pool();

    int m_MaxConn; //最大连接数
    int m_CurConn; //当前已使用的连接数
    int m_FreeConn; //当前空闲的连接数
    locker lock;
    std::list<MYSQL *> connList;//连接池
    sem reserve;


public:
    std::string m_url; //主机地址
    uint16_t m_Port; //数据库端口号
    std::string m_User; //数据库用户名
    std::string m_PassWord; //数据库密码
    std::string m_DatabaseName; //数据库名称
    int m_close_log;    //日志开关
};

class connectionRAII
{
public:
    connectionRAII(MYSQL **conn, connection_pool *connPool);
    ~connectionRAII();

private:
    MYSQL *conRAII;
    connection_pool *poolRAII;
};

#endif
