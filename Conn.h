#ifndef CONN_H_
#define CONN_H_

#include "common.h"

class Conn
{
public:
    Conn();
    ~Conn();

public:
    void Close();

public:
    int userid;
    struct bufferevent *bev;    
};

#endif //CONN_H_

