#include "UserConn.h"

UserConn::UserConn()
{
}

UserConn::~UserConn()
{
}

Conn *UserConn::FindUser(const std::string &userid)
{
    std::map<std::string, Conn *>::iterator ite =  m_Conn.find(userid);
    if(ite != m_Conn.end())
        return ite->second;
    return nullptr;
}

Conn *UserConn::FindUser(void *pBev)
{
    std::map<std::string, Conn *>::iterator ite =  m_Conn.begin();
    while(ite != m_Conn.end())
    {
        Conn *pConn = ite->second;
        if(pConn->m_pBev == pBev)
            return pConn;
        ++ite;
    } 
    return nullptr;
}

int UserConn::AddUser(Conn *pConn)
{
    if(!UserConn::FindUser(pConn->m_UserId))
        m_Conn.insert(std::pair<std::string, Conn *>(pConn->m_UserId, pConn));
    return 0;
}

int UserConn::RemoveUser(const std::string &userid)
{
    m_Conn.erase(userid);
    return 0;
}

