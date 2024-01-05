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

}
void WebServer::eventLoop()
{

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

}
void WebServer::dealwithsignal(bool &timeout, bool &stop_server)
{

}
void WebServer::withthread(int sockfd)
{

}
 void WebServer::dealwithwrite(int sockfd)
 {
    
 }