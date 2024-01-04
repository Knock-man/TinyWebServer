#include "log/log.h"
#include "mysql/mysql.h"
#include "lock/locker.h"
using namespace std;

int main()
{
    int m_close_log =0;
    Log::get_instance()->init("ServerLog", m_close_log, 2000, 2000,0);
    LOG_INFO("LOG_INFO");
    LOG_ERROR("LOG_ERROR");
    LOG_DEBUG("LOG_DEBUG");
    LOG_WARN("LOG_WARN");
     LOG_INFO("LOG_INFO");
    LOG_ERROR("LOG_ERROR");
    LOG_DEBUG("LOG_DEBUG");
    LOG_WARN("LOG_WARN");
     LOG_INFO("LOG_INFO");
    LOG_ERROR("LOG_ERROR");
    LOG_DEBUG("LOG_DEBUG");
    LOG_WARN("LOG_WARN");
    
    return 0;
}