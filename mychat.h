#include "tcp.h"

class Work
{
public:
	Work(struct event_base *pbase);
	~Work();

public:
	int Init();
	int Start();

public:
	struct event_base *m_pBase;
	struct event timeout;

private:
	TcpServer *m_TcpServer;
};

