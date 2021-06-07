#include "Conn.h"

Conn::Conn(const std::string &userId, struct bufferevent *pBev)
    : m_UserId(userId)
    , m_pBev(pBev)
{
}

Conn::~Conn()
{
}

void Conn::Close()
{
    fprintf(stderr, "conn close bev:%p\n", m_pBev);
    bufferevent_free(m_pBev);
}

int Conn::Send(const std::string &buff)
{
    int ret = bufferevent_write(m_pBev, buff.data(), buff.size());
    return ret;
}

int Conn::Send(const char *buff, int buffLen)
{
    int ret = bufferevent_write(m_pBev, buff, buffLen);
    return ret;
}

