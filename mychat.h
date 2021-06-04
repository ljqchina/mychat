#include "tcp.h"
//#include "Conn.h"
#include <map>

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
	TcpServer *m_TcpServer;
};

