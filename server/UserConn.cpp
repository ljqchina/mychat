#include "UserConn.h"

UserConn::UserConn()
{
}

UserConn::~UserConn()
{
}

Conn *UserConn::FindUser(int userid)
{
    std::map<int, Conn *>::iterator ite =  m_Conn.find(userid);
    if(ite != m_Conn.end())
        return ite->second;
    return nullptr;
}

Conn *UserConn::FindUser(void *pBev)
{
    std::map<int, Conn *>::iterator ite =  m_Conn.begin();
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
        m_Conn.insert(std::pair<int, Conn *>(pConn->m_UserId, pConn));
    return 0;
}

int UserConn::RemoveUser(int userid)
{
    m_Conn.erase(userid);
    return 0;
}

