#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <string>

#include "Protocol.h"
#include "msgtype.h"
#include "errcode.h"
#include "util.h"

using namespace std;

static int bexit = 0;
void signal_handle(int sig)
{
	printf("SIG:%d\n", sig);
	if(sig == SIGINT)
	{
		bexit = 1;
		printf("SIGINT exit\n");
	}
}

int myconnect(const char *ip, int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0); 
	if(sock < 0)
	{
		perror("socket");
		return -1;
	}

	struct sockaddr_in addr; 
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(port); 
	addr.sin_addr.s_addr = inet_addr(ip); 

	socklen_t addr_len = sizeof(addr);
	int fd = connect(sock, (struct sockaddr*)&addr, addr_len);
	if(fd < 0)
	{
		perror("connect");
		return -1;
	}
	return sock;
}

//注册消息
void user_register(int sock)
{
    string msg;
    Protocol pt;
    RegisterInfo ri;

    ri.header.version = "1.0.0.0";
    ri.header.msgType = USER_REG_REQ;
    ri.userId = "13000000002";
    ri.nickName = "abc";
    ri.password = util::md5("123");
    ri.sex = 1;
    ri.age = 20;

    pt.PackRegister(ri, msg);
    send(sock, msg.data(), msg.size(), 0);

    fprintf(stderr, "%s request:%s\n\n", __func__, msg.data());

	//receive response msg
	char data[1024];
	memset(data, 0, sizeof(data));
	recv(sock, data, 1000, 0);

    fprintf(stderr, "%s response:%s\n", __func__, data);

	msg = std::string(data);
	pt.ParseRegister(ri, msg);
	fprintf(stderr, "respcode:%d[%s]\n", ri.header.respCode, errstr(ri.header.respCode).data());

	fprintf(stderr, "---------------------------------------\n\n");
}

//登录消息
void user_login(int sock)
{
    string msg;
    Protocol pt;
    LoginInfo info;

    info.header.version = "1.0.0.0";
    info.header.msgType = USER_LOGIN_REQ;
    info.userId = "13000000001";
    info.password = "123";
    info.termType = 1;

    pt.PackLogin(info, msg);
    send(sock, msg.data(), msg.size(), 0);

    fprintf(stderr, "%s request:%s\n\n", __func__, msg.data());

	//receive response msg
	char data[4096];
	memset(data, 0, sizeof(data));
	recv(sock, data, 4, 0);
	int len = atoi(data);
	memset(data, 0, sizeof(data));
	recv(sock, data, len, 0);

    fprintf(stderr, "%s response:%s\n", __func__, data);

	msg = std::string(data);
	pt.ParseLogin(info, msg);
	fprintf(stderr, "respcode:%d[%s]\n", info.header.respCode, errstr(info.header.respCode).data());

	fprintf(stderr, "---------------------------------------\n\n");

	/*
	memset(data, 0, sizeof(data));
	recv(sock, data, 4, 0);
	len = atoi(data);
	memset(data, 0, sizeof(data));
	recv(sock, data, len, 0);
    fprintf(stderr, "%s response:%d[%s]\n", __func__, len, data);
	msg = std::string(data);
	std::vector<OfflineInfo> vec;
	pt.ParseOfflineMsg(vec, msg);
    fprintf(stderr, "%s offlinemsg size:%d\n", __func__, vec.size());
	*/
}

//注销消息
void user_logout(int sock)
{
    string msg;
    Protocol pt;
    LogoutInfo info;

    info.header.version = "1.0.0.0";
    info.header.msgType = USER_LOGOUT_REQ;
    info.userId = "13000000001";
    info.termType = 1;

    pt.PackLogout(info, msg);
    send(sock, msg.data(), msg.size(), 0);

    fprintf(stderr, "%s request:%s\n\n", __func__, msg.data());

	//receive response msg
	char data[1024];
	memset(data, 0, sizeof(data));
	recv(sock, data, 1000, 0);

    fprintf(stderr, "%s response:%s\n", __func__, data);

	msg = std::string(data);
	pt.ParseLogout(info, msg);
	fprintf(stderr, "respcode:%d[%s]\n", info.header.respCode, errstr(info.header.respCode).data());

	fprintf(stderr, "---------------------------------------\n\n");
}

//添加好友请求
void user_addfriend_req(int sock)
{
    string msg;
    Protocol pt;
    AddFriendInfo info;

    info.header.version = "1.0.0.0";
    info.header.msgType = USER_ADDFRIEND_REQ;
    info.userId = "13000000001";
    info.userId_to = "13000000002";
    info.content = "添加好友";

    pt.PackAddFriend(info, msg);
    send(sock, msg.data(), msg.size(), 0);

    fprintf(stderr, "%s request:%s\n\n", __func__, msg.data());

	//receive response msg
	char data[1024];
	memset(data, 0, sizeof(data));
	recv(sock, data, 4, 0);
	int len = atoi(data);
	memset(data, 0, sizeof(data));
	recv(sock, data, len, 0);

    fprintf(stderr, "%s response:%s\n", __func__, data);

	msg = std::string(data);
	pt.ParseAddFriend(info, msg);
	if(info.header.msgType == USER_ADDFRIEND_REQ)
		fprintf(stderr, "recved add friend request:%d\n", info.header.msgType);
	if(info.header.msgType == USER_ADDFRIEND_RESP)
		fprintf(stderr, "recved add friend result:%d[%d]\n", info.header.msgType, info.flag); 
	fprintf(stderr, "---------------------------------------\n\n");
}

//添加好友响应消息
void user_addfriend_resp(int sock)
{
    string msg;
    Protocol pt;
    AddFriendInfo info;

    info.header.version = "1.0.0.0";
    info.header.msgType = USER_ADDFRIEND_RESP;
    info.userId = "13000000002";
    info.userId_to = "13000000001";
	info.flag = 1;

    pt.PackAddFriend(info, msg);
    send(sock, msg.data(), msg.size(), 0);

    fprintf(stderr, "%s request:%s\n\n", __func__, msg.data());

	//receive response msg
	char data[1024];
	memset(data, 0, sizeof(data));
	recv(sock, data, 4, 0);
	int len = atoi(data);
	memset(data, 0, sizeof(data));
	recv(sock, data, len, 0);

    fprintf(stderr, "%s response:%s\n", __func__, data);

	msg = std::string(data);
	pt.ParseAddFriend(info, msg);
	if(info.header.msgType == USER_ADDFRIEND_REQ)
		fprintf(stderr, "recved add friend request:%d\n", info.header.msgType);
	if(info.header.msgType == USER_ADDFRIEND_RESP)
		fprintf(stderr, "recved add friend result:%d[%d]\n", info.header.msgType, info.flag); 
	fprintf(stderr, "---------------------------------------\n\n");
}

//删除好友请求
void user_delfriend_req(int sock)
{
    string msg;
    Protocol pt;
    DelFriendInfo info;

    info.header.version = "1.0.0.0";
    info.header.msgType = USER_DELFRIEND_REQ;
    info.userId = "13000000001";
    info.userId_to = "13000000002";

    pt.PackDelFriend(info, msg);
    send(sock, msg.data(), msg.size(), 0);

    fprintf(stderr, "%s request:%s\n\n", __func__, msg.data());

	//receive response msg
	char data[1024];
	memset(data, 0, sizeof(data));
	recv(sock, data, 4, 0);
	int len = atoi(data);
	memset(data, 0, sizeof(data));
	recv(sock, data, len, 0);

    fprintf(stderr, "%s response:%s\n", __func__, data);

	msg = std::string(data);
	pt.ParseDelFriend(info, msg);
	fprintf(stderr, "recved del friend result:%d[%d]\n", info.header.msgType, info.header.respCode); 
	fprintf(stderr, "---------------------------------------\n\n");
}

//删除好友响应
void user_delfriend_resp(int sock)
{
    string msg;
    Protocol pt;
    DelFriendInfo info;

    info.header.version = "1.0.0.0";
    info.header.msgType = USER_DELFRIEND_RESP;
    info.userId = "13000000002";
    info.userId_to = "13000000001";

    pt.PackDelFriend(info, msg);
    send(sock, msg.data(), msg.size(), 0);

    fprintf(stderr, "%s request:%s\n\n", __func__, msg.data());
}

//好友聊天请求
void user_chat_req(int sock)
{
    string msg;
    Protocol pt;
    ChatInfo info;

    info.header.version = "1.0.0.0";
    info.header.msgType = USER_CHAT_REQ;
    info.userId = "13000000001";
    info.userId_to = "13000000002";
	info.content = "hello everyone!";

    pt.PackChat(info, msg);
    send(sock, msg.data(), msg.size(), 0);

    fprintf(stderr, "%s request:%s\n\n", __func__, msg.data());
}


/***************************************************************/
/***************************************************************/
//MAIN FUNCTION
/////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	if(argc != 3)
	{
		fprintf(stderr, "usage ./tc ip port\n");
		return 0;
	}

	signal(SIGPIPE, signal_handle);
	signal(SIGINT, signal_handle);

	char ip[32];
	int port;

	strcpy(ip, argv[1]);
	port = atoi(argv[2]);
	if(port <= 0 || port > 65535)
	{
		fprintf(stderr, "invalid port\n");
		return 0;
	}

	int i = 0;
	int sock = 0;
	while((sock = myconnect(ip, port)) < 0)
	{
		printf("reconnectting %d ...\n", ++i);
		sleep(2);
	}

    //1. 用户注册
    //user_register(sock);
	//sleep(3);

	//2. 用户登录
    user_login(sock);
	sleep(3);

	//3. 用户注销
    //user_logout(sock);

	//4. 添加好友请求
	//user_addfriend_req(sock);

	//5. 添加好友响应
	//user_addfriend_resp(sock);

	//6. 删除好友
	//user_delfriend_req(sock);

	//7. 删除好友响应
	//user_delfriend_resp(sock);

	//8. 好友聊天消息
	//user_chat_req(sock);

	close(sock);
	printf("end\n");
	return 0;
}

