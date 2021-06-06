#include <string.h>
#include "MyChat.h"
#include "msgtype.h"
#include "errcode.h"

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
        case USER_REG_REQ:
        {
            RegisterInfo ri; 
            if(pmc->m_Protocol.ParseRegister(ri, msg) != 0)
                break;
            Conn *pConn = pmc->m_UserConn.FindUser(ri.userId);
            if(pConn == nullptr)
            {
				pConn = new Conn(ri.userId, bev);
                pmc->m_UserConn.AddUser(pConn);
                fprintf(stderr, "new user:%p, userid:%s, nickname:%s\n", bev, ri.userId.data(), ri.nickName.data());
            }
            else
            {
                fprintf(stderr, "old user:%p\n", bev);
            }

			//response
			ri.header.msgType = USER_REG_RESP;
			ri.header.respCode = 0;
			ri.header.respText = "success";
			pmc->m_Protocol.PackRegister(ri, msg);
			fprintf(stderr, "register response:%s\n", msg.data());
			pConn->Send(msg);
            break;
        }
        //登录
        case USER_LOGIN_REQ:
        {
            LoginInfo info; 
            if(pmc->m_Protocol.ParseLogin(info, msg) != 0)
                break;
            Conn *pConn = pmc->m_UserConn.FindUser(info.userId);
            if(pConn == nullptr)
            {
				pConn = new Conn(info.userId, bev);
                pmc->m_UserConn.AddUser(pConn);
                fprintf(stderr, "login user:%p\n", bev);
            }
            else
            {
                fprintf(stderr, "login already user:%p\n", bev);
            }

			//response
			info.header.msgType = USER_LOGIN_RESP;
			info.header.respCode = 0;
			info.header.respText = "success";
			pmc->m_Protocol.PackLogin(info, msg);
			fprintf(stderr, "login response:%s\n", msg.data());
			pConn->Send(msg);
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

