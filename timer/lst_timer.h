#ifndef LST_TIMER_H
#define LST_TIMER_H

#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "../log/log.h"

class util_timer;

struct client_data
{
    sockaddr_in address;
    int sockfd;
    util_timer *timer;
};

class util_timer
{
public:
    util_timer() : prev(NULL), next(NULL){}
    

public:
    time_t expire;

    void (* cb_func)(client_data *);
    client_data *user_data;
    util_timer *prev;
    util_timer *next;
};
#endif