#include <map>
#include <string>
#include "Tcp.h"
#include "UserConn.h"
#include "Protocol.h"

class MyChat 
{
public:
	MyChat(struct event_base *pbase);
	~MyChat();

public:
	int Init();
	int Start();
	int ProHeart(struct bufferevent *bev, const std::string &msg);
	int ProRegister(struct bufferevent *bev, const std::string &msg);
	int ProLogin(struct bufferevent *bev, const std::string &msg);
	int ProLogout(struct bufferevent *bev, const std::string &msg);
	int ProAddFriend(struct bufferevent *bev, const std::string &msg);
	int ProDelFriend(struct bufferevent *bev, const std::string &msg);
	int ProChat(struct bufferevent *bev, const std::string &msg);

public:
	struct event_base *m_pBase;
	struct event timeout;
	UserConn m_UserConn;
 	Protocol m_Protocol;

private:
    //tcp服务器
	TcpServer *m_TcpServer;
};

