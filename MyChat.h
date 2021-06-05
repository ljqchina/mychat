#include <map>
#include "Tcp.h"
#include "Conn.h"

class MyChat 
{
public:
	MyChat(struct event_base *pbase);
	~MyChat();

public:
	int Init();
	int Start();

public:
	struct event_base *m_pBase;
	struct event timeout;

private:
    //tcp服务器
	TcpServer *m_TcpServer;

public:
    //在线用户连接
    std::map<struct bufferevent *, Conn *> m_Conn;
};

