#include "Conn.h"

Conn::Conn()
{
}

Conn::~Conn()
{
}

void Conn::Close()
{
    bufferevent_free(bev);
}


