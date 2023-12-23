#include "sql_connection_pool.h"

connection_pool::connection_pool():
    m_CurConn(0),
    m_FreeConn(0)
{

}
connection_pool::~connection_pool()
{
    DestroyPoll();
}


MYSQL *connection_pool::GetConnection()
{

    if(0 == connList.size())return NULL;

    reserve.wait();

    lock.lock();

     MYSQL *conn = connList.front();
     connList.pop_front();

     --m_FreeConn;
     ++m_CurConn;

     lock.unlock();

     return conn;

}

//归还连接
bool connection_pool::ReleaseConnection(MYSQL *conn)
{
    if(NULL == conn) return false;

    lock.lock();

    connList.push_back(conn);

    m_FreeConn++;
    m_CurConn--;

    lock.unlock();

    reserve.post();

    return true;
}

//当前空闲的连接数
int connection_pool::GetFreeConn()
{
    return this->m_FreeConn;

}
   
//销毁数据库连接池
void connection_pool::DestroyPoll()
{
    lock.lock();
    if(connList.size() > 0)
    {
        auto it = connList.begin();
        for(; it!=connList.end(); it++)
        {
            MYSQL *con = *it;
            mysql_close(con);//关闭这个连接
        }
        m_CurConn = 0;
        m_FreeConn = 0;
        connList.clear();

    }
    lock.unlock();

}

//单例模式
connection_pool *connection_pool::GetInstance()
{
    static connection_pool connpool;
    return &connpool;

}

void connection_pool::init(std::string url ,std::string User, std::string Password,std::string DBName, uint16_t Port, int MaxConn, int close_log)
{
    m_url = url;
    m_Port = Port;
    m_User = User;
    m_PassWord = Password;
    m_DatabaseName = DBName;
    m_close_log = close_log;
    //初始化连接
    for(int i = 0; i < MaxConn; i++)
    {
        MYSQL *con = NULL;
        con = mysql_init(con);

        if(con == NULL)
        {
            //LOG_ERROR("mysql_init error");
            std::cout<<"mysql_init error"<<std::endl;
            exit(1);
        }
 
        con = mysql_real_connect(con,url.c_str(),User.c_str(),Password.c_str(),DBName.c_str(),Port,NULL,0);

        if(con == NULL)
        {
            //LOG_ERROR("mysql_real_connect error");
            std::cout<<"mysql_real_connect error"<<std::endl;
            exit(1);
        }

        connList.push_back(con);//连接存入列表中
        ++m_FreeConn;
       
    }

    reserve = sem(m_FreeConn);//注册信号量

    m_MaxConn = m_FreeConn;
}

connectionRAII::connectionRAII(MYSQL **SQL, connection_pool *connPool)
{
    *SQL = connPool->GetConnection();

    conRAII = *SQL;
    poolRAII = connPool;
}
connectionRAII::~connectionRAII()
{
    poolRAII->ReleaseConnection(conRAII);
}

