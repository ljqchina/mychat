#include <map>
#include "Tcp.h"
#include "UserConn.h"

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
    UserConn m_UserConn;

private:
    //tcp服务器
	TcpServer *m_TcpServer;
};

