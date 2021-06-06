#ifndef USER_CONN_H_
#define USER_CONN_H_

#include <map>
#include <string>
#include "Conn.h"

class UserConn
{
public:
    UserConn();
    ~UserConn();

public:
    Conn *FindUser(const std::string &userid);
    Conn *FindUser(void *pBev);
    int AddUser(Conn *pConn);
    int RemoveUser(const std::string &userid);

private:
    std::map<std::string, Conn *> m_Conn;
};

#endif

