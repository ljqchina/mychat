#ifndef USER_CONN_H_
#define USER_CONN_H_

#include <map>
#include "Conn.h"

class UserConn
{
public:
    UserConn();
    ~UserConn();

public:
    Conn *FindUser(int userid);
    Conn *FindUser(void *pBev);
    int AddUser(Conn *pConn);
    int RemoveUser(int userid);

private:
    std::map<int, Conn *> m_Conn;
};

#endif

