#include <string.h>
#include "MyChat.h"

using namespace std;

static void timeout_cb(evutil_socket_t fd, short event, void *arg)
{
	fprintf(stderr, "timerout...\n");
}

static void OnRead(struct bufferevent *bev, void *arg)
{
	MyChat *pmc = static_cast<MyChat *>(arg);
	char data[1024];
	memset(data, 0, sizeof(data));
	bufferevent_read(bev, data, 100);
	fprintf(stderr, "%s:[%s]\n", __func__, data);
	fprintf(stderr, "--------------------------\n");

    //解析消息

    //注册

    //登录
    int userid = 1;
    Conn *pConn = pmc->m_UserConn.FindUser(userid);
    if(pConn == nullptr)
    {
        pmc->m_UserConn.AddUser(new Conn(userid, bev));
        fprintf(stderr, "new user:%p\n", bev);
    }
    else
    {
        fprintf(stderr, "old user:%p\n", bev);
    }

    //
}

static void OnClose(struct bufferevent *bev, void *arg)
{
	MyChat *pmc = static_cast<MyChat *>(arg);
	fprintf(stderr, "OnClose\n");
	fprintf(stderr, "==========================\n");

    int userid = 1;
    Conn *pConn = pmc->m_UserConn.FindUser(bev);
    if(pConn != nullptr)
    {
        fprintf(stderr, "user logout:%p\n", bev);
        pConn->Close();
        pmc->m_UserConn.RemoveUser(pConn->m_UserId);
    }
}

MyChat::MyChat(struct event_base *base)
	: m_pBase(base)
	, m_TcpServer(nullptr)
{
}

MyChat::~MyChat()
{
	if(m_TcpServer)
		delete m_TcpServer;
}

int MyChat::Init()
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

int MyChat::Start()
{
	m_TcpServer->Start(19000);
	return 0;
}

