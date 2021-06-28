#include <string.h>
#include "MyChat.h"
#include "msgtype.h"
#include "errcode.h"
#include "db.h"
#include "util.h"

using namespace std;

static void timeout_cb(evutil_socket_t fd, short event, void *arg)
{
	fprintf(stderr, "timerout...\n");
}

static void OnRead(struct bufferevent *bev, void *arg)
{
	MyChat *pmc = static_cast<MyChat *>(arg);
	char data[4096];
	while(1)
	{
		memset(data, 0, sizeof(data));
		bufferevent_read(bev, data, PACKAGE_HEADER_LEN);
		int len = atoi(data);
		if(len >= 4096)
		{
			fprintf(stderr, "%s, Package Length error:%d\n", __func__, len);
			return;
		}
		if(len <= 0)
		{
			fprintf(stderr, "%s, No more Package data to receive:%d\n", __func__, len);
			return;
		}
		memset(data, 0, sizeof(data)); bufferevent_read(bev, data, len); fprintf(stderr, "%s:[%d][%s]\n", __func__, len, data);
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
			//添加好友请求和好友响应
			case USER_ADDFRIEND_REQ:
			case USER_ADDFRIEND_RESP:
			{
				pmc->ProAddFriend(bev, msg);
				break;
			}
			default:
				fprintf(stderr, "msgType error:%d\n", h.msgType);
				break;
		}
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
	info.header.respCode = ERR_SUCCESS;
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

	//生成一个连接对象,注册连接对象不保存,使用完后释放掉,登录时才保存
	Conn *pConn = m_UserConn.FindUser(info.userId);
	if(pConn != nullptr)
	{
		fprintf(stderr, "repeat register user:%p\n", bev);
		return 0;
	}
	pConn = new Conn(info.userId, bev);
	//m_UserConn.AddUser(pConn);	//注册连接不保存,登录时才保存

	//检查是否已经注册,如果已经注册提示已经注册
	if(db::user::IsRegistered(info.userId))
	{
		fprintf(stderr, "user:%p, %s is already a register user\n", bev, info.userId.data());
		std::string str;
		info.header.msgType = USER_REG_RESP;
		info.header.respCode = ERR_USER_EXISTS;
		m_Protocol.PackRegister(info, str);
		pConn->Send(str);
		delete pConn;
		return 0;
	}

	//未注册则进行注册,存储到数据库用户信息表
	info.type = 0;
	info.status = 0;
	if(db::user::RegisterUser(info) != 0)
	{
		fprintf(stderr, "db::user::RegisterUser failed, userid:%s\n", info.userId.data());
		delete pConn;
		return -1;
	}

	fprintf(stderr, "new register user:%p, userid:%s, nickname:%s\n", bev, info.userId.data(), info.nickName.data());

	//response
	std::string str;
	info.header.msgType = USER_REG_RESP;
	info.header.respCode = ERR_SUCCESS;
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

	//检查用户连接对象是否存在
	//存在说明已经登录需要判断是否多个机器登录同一账户
	Conn *pConn = m_UserConn.FindUser(info.userId);
	if(pConn == nullptr)
	{
		pConn = new Conn(info.userId, bev);
		fprintf(stderr, "login user:%p\n", bev);
	}
	else
	{
		//如果已经登录,需要判断bev是否相等,不相等则认为一个账户在多个终端登录,
		//或者之前的连接状态异常(比如网络异常),需要将之前的连接断开清除掉
		if(pConn->Bev() == bev)
		{
			//重复登录,应该不存在这个情况
			fprintf(stderr, "repeat login user:%p\n", bev);
			return 0;
		}
		else
		{
			fprintf(stderr, "login other machine, user:%p\n", bev);

			//多个机器登录同一账户,通知先前登录的机器下线即客户端主动关闭连接
			std::string str;
			info.header.msgType = USER_LOGIN_RESP;
			info.header.respCode = ERR_USER_LOGIN_OTHER;
			m_Protocol.PackLogin(info, str);
			pConn->Send(str);
			//这里仅移除和释放用户连接对象Conn,Conn保存的bev在服务器收到关闭事件时再关闭
			m_UserConn.RemoveUser(info.userId);
			delete pConn;

			//本次登录生成新的连接对象,现在不保存,登录成功后保存
			pConn = new Conn(info.userId, bev); 
		}
	}

	//检查是否已注册,非注册用户不能登录
	if(!db::user::IsRegistered(info.userId))
	{
		fprintf(stderr, "user:%p, %s is not a register user\n", bev, info.userId.data());
		std::string str;
		info.header.msgType = USER_LOGIN_RESP;
		info.header.respCode = ERR_USER_NOT_EXISTS;
		m_Protocol.PackLogin(info, str);
		pConn->Send(str);
		delete pConn;
		return 0;
	}

	//response setting
	std::string str;
	info.header.msgType = USER_LOGIN_RESP;

	//验证用户密码
	info.password = util::md5(info.password);
	fprintf(stderr, "password:%s\n", info.password.data());
	if(!db::user::CheckPassword(info))
	{
		info.header.respCode = ERR_USER_ACOUNT_PASS;
		m_Protocol.PackLogin(info, str);
		pConn->Send(str);
		fprintf(stderr, "login password error, response:%s\n", str.data());
		return 0;
	}

	//发送登录成功消息
	info.header.respCode = ERR_SUCCESS;
	m_Protocol.PackLogin(info, str);
	fprintf(stderr, "login response:%s\n", str.data());
	pConn->Send(str);
	m_UserConn.AddUser(pConn); //保存用户连接对象

	//查询该用户的离线消息并发送
	std::vector<OfflineInfo> vec;
	db::user::QueryOfflineMsg(info.userId, vec);
	if(vec.size() == 0)
		return 0;
	for(auto &v : vec)
	{
		str = v.content;
		m_Protocol.PackOfflineMsg(str);
		pConn->Send(str);
		fprintf(stderr, "Send OfflineMsg:%s\n", str.data());
	}

	return 0;
}

//处理注销消息
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
	}

	//response
	std::string str;
	info.header.msgType = USER_LOGOUT_RESP;
	info.header.respCode = ERR_SUCCESS;
	m_Protocol.PackLogout(info, str);
	fprintf(stderr, "logout response:%s\n", str.data());
	pConn->Send(str);
	//这里仅移除和释放用户连接对象Conn,Conn保存的bev在服务器收到关闭事件时再关闭
	m_UserConn.RemoveUser(info.userId);
	delete pConn;
	return 0;
}

//处理添加好友请求和好友响应消息
int MyChat::ProAddFriend(struct bufferevent *bev, const std::string &msg)
{
	AddFriendInfo info; 
	if(m_Protocol.ParseAddFriend(info, msg) != 0)
		return -1;

	//如果是好友响应消息,则判断同意/拒绝,如果同意则建立好友关系到好友数据表friend
	if(info.header.msgType == USER_ADDFRIEND_RESP && info.flag == OPERATE_FRIEND_AGREE)
	{
		//检查是否已是好友

		//互为好友
		db::user::MakeFriend(info.userId_to, info.userId);
		db::user::MakeFriend(info.userId, info.userId_to);
		fprintf(stderr, "makefriend\n");
	}

	//查找目的用户
	Conn *pConn = m_UserConn.FindUser(info.userId_to);
	if(pConn == nullptr)
	{
		//目的用户未登录,需要将好友请求或者好友响应保存到离线消息表
		fprintf(stderr, "addfriend user not login, save offline msg:%s\n", info.userId_to.data());
		OfflineInfo oi;
		oi.userId = info.userId_to;
		oi.content = msg;
		oi.datetime = util::DateTime();
		oi.status = 0;//暂时未用默认0
		//保存离线消息
		db::user::SaveOfflineMsg(oi);
		return 0;
	}
	else
	{
		//找到目的用户,直接转发好友请求或者好友响应到目的用户
		fprintf(stderr, "send add friend request/result to user:%s\n", info.userId_to.data());
		pConn->Send(msg);
	}
	return 0;
}

