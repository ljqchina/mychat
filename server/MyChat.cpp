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
	bufferevent_read(bev, data, 1000);
	fprintf(stderr, "%s:[%s]\n", __func__, data);
	fprintf(stderr, "--------------------------\n");

    std::string msg(data);

    //解析消息类型
    Header h;
    pmc->m_Protocol.ParseHeader(msg, h);
    fprintf(stderr, "msgType:%d, version:%s\n", h.msgType, h.version.data());

    switch(h.msgType)
    {
        //注册
        case 2001:
        {
            RegisterInfo ri; 
            if(pmc->m_Protocol.ParseRegisterReq(ri, msg) != 0)
                break;
            int userid = 1;
            Conn *pConn = pmc->m_UserConn.FindUser(userid);
            if(pConn == nullptr)
            {
                pmc->m_UserConn.AddUser(new Conn(userid, bev));
                fprintf(stderr, "new user:%p, userid:%s, nickname:%s\n", bev, ri.userId.data(), ri.nickName.data());
            }
            else
            {
                fprintf(stderr, "old user:%p\n", bev);
            }
            break;
        }
        //登录
        case 2003:
        {
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
            break;
        }
        default:
            fprintf(stderr, "msgType error:%d\n", h.msgType);
            break;
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

