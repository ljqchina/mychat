#ifndef CONN_H_
#define CONN_H_

//#include "common.h"

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/event_struct.h>

#include <string>

class Conn
{
public:
    Conn(const std::string &userId, struct bufferevent *pBev);
    ~Conn();

public:
    void Close();
    int Send(const std::string &buff);
    int Send(const char *buff, int buffLen);

public:
    time_t m_LastTime;
    std::string m_UserId;
    struct bufferevent *m_pBev;    
};

#endif //CONN_H_

