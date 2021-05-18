#include <string.h>
#include "work.h"

using namespace std;

static void timeout_cb(evutil_socket_t fd, short event, void *arg)
{
	fprintf(stderr, "timerout...\n");
}

static void OnRead(struct bufferevent *bev, void *arg)
{
	Work *pwk = static_cast<Work *>(arg);
	char data[1024];
	memset(data, 0, sizeof(data));
	bufferevent_read(bev, data, 100);
	fprintf(stderr, "%s:[%s]\n", __func__, data);
	fprintf(stderr, "--------------------------\n");
}

static void OnClose(struct bufferevent *bev, void *arg)
{
	Work *pwk = static_cast<Work *>(arg);
	fprintf(stderr, "OnClose\n");
	fprintf(stderr, "==========================\n");
}

Work::Work(struct event_base *base)
	: m_pBase(base)
	, m_TcpServer(nullptr)
{
}

Work::~Work()
{
	if(m_TcpServer)
		delete m_TcpServer;
}

int Work::Init()
{
	//add timer
	/*
	struct timeval tv;
	tv.tv_sec = 3;
	event_assign(&timeout, m_pBase, -1, EV_PERSIST, timeout_cb, this);
	evtimer_add(&timeout, &tv);
	*/

	//start tcp server
	m_TcpServer = new TcpServer(m_pBase);
	m_TcpServer->SetReadcb(OnRead, this);
	m_TcpServer->SetClosecb(OnClose, this);

	return 0;
}

int Work::Start()
{
	m_TcpServer->Start(19000);
}

