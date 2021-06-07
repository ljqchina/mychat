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
			pmc->ProRegister(bev, msg);	
            break;
        }
        //登录
        case USER_LOGIN_REQ:
        {
			pmc->ProLogin(bev, msg);
	        break;
        }
		//注销
		case USER_LOGOUT_REQ:
		{
			pmc->ProLogout(bev, msg);
			break;
		}
		//心跳
		case SYS_HEART_REQ:
		{
			pmc->ProHeart(bev, msg);
			break;
		}
        default:
            fprintf(stderr, "msgType error:%d\n", h.msgType);
            break;
    }
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
        pmc->m_UserConn.RemoveUser(pConn->m_UserId);
        pConn->Close();
		delete pConn;
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

int MyChat::ProHeart(struct bufferevent *bev, const std::string &msg)
{
	HeartInfo info; 
	if(m_Protocol.ParseHeart(info, msg) != 0)
		return -1;
	Conn *pConn = m_UserConn.FindUser(info.userId);
	if(pConn == nullptr)
	{
		fprintf(stderr, "heart msg, no user:%p, userid:%s\n", bev, info.userId.data());
		return 0;
	}
	else
	{
		fprintf(stderr, "old user:%p\n", bev);
	}

	//response
	std::string str;
	info.header.msgType = SYS_HEART_RESP;
	info.header.respCode = 0;
	info.header.respText = "success";
	m_Protocol.PackHeart(info, str);
	fprintf(stderr, "heart msg response:%s\n", str.data());
	pConn->Send(str);
	return 0;
}

//处理注册消息
int MyChat::ProRegister(struct bufferevent *bev, const std::string &msg)
{
	RegisterInfo info; 
	if(m_Protocol.ParseRegister(info, msg) != 0)
		return -1;

	//检查是否已经注册,如果已经注册提示已经注册


	//未注册则进行注册
	Conn *pConn = m_UserConn.FindUser(info.userId);
	if(pConn != nullptr)
	{
		fprintf(stderr, "repeat register user:%p\n", bev);
		return 0;
	}

	pConn = new Conn(info.userId, bev);
	//m_UserConn.AddUser(pConn);	//注册连接不保存,登录时才保存
	fprintf(stderr, "new register user:%p, userid:%s, nickname:%s\n", bev, info.userId.data(), info.nickName.data());

	//response
	std::string str;
	info.header.msgType = USER_REG_RESP;
	info.header.respCode = 0;
	info.header.respText = "success";
	m_Protocol.PackRegister(info, str);
	fprintf(stderr, "register response:%s\n", str.data());
	pConn->Send(str);
	delete pConn; //注册连接不保存,登录时才保存,释放掉,登录保存时再创建,注意，Conn保存的bev不用释放,用户注销或者断开连接时才释放
	return 0;
}

//处理登录消息
int MyChat::ProLogin(struct bufferevent *bev, const std::string &msg)
{
	LoginInfo info; 
	if(m_Protocol.ParseLogin(info, msg) != 0)
		return -1;

	//检查是否已注册,非注册用户不能登录


	//登录
	Conn *pConn = m_UserConn.FindUser(info.userId);
	if(pConn == nullptr)
	{
		pConn = new Conn(info.userId, bev);
		m_UserConn.AddUser(pConn);
		fprintf(stderr, "login user:%p\n", bev);
	}
	else
	{
		fprintf(stderr, "login already user:%p\n", bev);
		//如果已经登录,需要判断bev是否相等,不相等则认为一个账户在多个终端登录,
		//或者之前的连接状态异常(比如网络异常),需要将之前的连接断开清除掉

	}

	//response
	std::string str;
	info.header.msgType = USER_LOGIN_RESP;
	info.header.respCode = 0;
	info.header.respText = "success";
	m_Protocol.PackLogin(info, str);
	fprintf(stderr, "login response:%s\n", str.data());
	pConn->Send(str);
	return 0;
}

int MyChat::ProLogout(struct bufferevent *bev, const std::string &msg)
{
	LogoutInfo info; 
	if(m_Protocol.ParseLogout(info, msg) != 0)
		return -1;
	Conn *pConn = m_UserConn.FindUser(info.userId);
	if(pConn == nullptr)
	{
		fprintf(stderr, "not login user:%p\n", bev);
		return 0;
	}
	else
	{
		fprintf(stderr, "do logout user:%p\n", bev);
		m_UserConn.RemoveUser(info.userId);
	}

	//response
	std::string str;
	info.header.msgType = USER_LOGOUT_RESP;
	info.header.respCode = 0;
	info.header.respText = "success";
	m_Protocol.PackLogout(info, str);
	fprintf(stderr, "logout response:%s\n", str.data());
	pConn->Send(str);
	//pConn->Close(); //客户端发起关闭,或者心跳超时由服务器主动关闭
	return 0;
}

