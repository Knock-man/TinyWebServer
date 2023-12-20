#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include <iostream>

//连接池
class connection_pool
{
public:

private:
    std::string m_url; //主机地址
    std::string m_Port; //数据库端口号
    std::string m_User; //数据库用户名
    std::string m_PassWord; //数据库密码
    std::string m_DatabaseName; //数据库名称
    

};

#endif
