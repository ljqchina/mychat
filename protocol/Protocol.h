#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "stringbuffer.h"
#include "writer.h"
#include "document.h"

#include <string>
#include <vector>

#define PACKAGE_HEADER_LEN 4

typedef struct Header_
{
    int msgType;
    int respCode;
    std::string msgId;
    std::string version;
}Header;

typedef struct HeartInfo_
{
	Header header;
	std::string userId;
}HeartInfo;

typedef struct RegisterInfo_
{
    Header header;
    int sex;
    int age;
	int type;
	int status;
    std::string userId;
    std::string nickName;
    std::string password;
	std::string email;
	std::string mobile;
	std::string address;
}RegisterInfo;

typedef struct LoginInfo_
{
	Header header;
	int termType;
    std::string userId;
    std::string password;
}LoginInfo;

typedef struct LogoutInfo_
{
	Header header;
	int termType;
    std::string userId;
}LogoutInfo;

typedef struct OfflineInfo_
{
	int type;
	int status;
	int msgType;
	std::string userId;
	std::string userId_to;
	std::string content;
	std::string datetime;
}OfflineInfo;

typedef struct AddFriendInfo_
{
	Header header;
	int flag;
	std::string userId;
	std::string userId_to;
	std::string content;
}AddFriendInfo;

class Protocol
{
public:
    Protocol();
    ~Protocol();

public:
	//打包增加长度信息
	void PackLength(std::string &msg);

	//打包报文头
    int PackHeader(rapidjson::Writer<rapidjson::StringBuffer> &w, const Header &h);
	//响应报文增加响应字段
    int PackHeaderResp(rapidjson::Writer<rapidjson::StringBuffer> &w, const Header &h);
	//解析报文头
    int ParseHeader(const std::string &msg, Header &h);
    int ParseHeader(const rapidjson::Document &doc, Header &h);

	//解析头部响应字段从json对象
	int ParseHeaderResp(const rapidjson::Document &doc, Header &h);

	//打包心跳包消息
	int PackHeart(const HeartInfo &info, std::string &msg);
	//解析心跳包消息
	int ParseHeart(HeartInfo &info, const std::string &msg);

	//打包注册用户报文
    int PackRegister(const RegisterInfo &info, std::string &msg);
	//解析注册用户报文
    int ParseRegister(RegisterInfo &info, const std::string &msg);

	//打包登录报文
    int PackLogin(const LoginInfo &info, std::string &msg);
	//解析登录报文
    int ParseLogin(LoginInfo &info, const std::string &msg);

	//打包注销报文
    int PackLogout(const LogoutInfo &info, std::string &msg);
	//解析注销报文
    int ParseLogout(LogoutInfo &info, const std::string &msg);

	//打包离线消息
	int PackOfflineMsg(const std::vector<OfflineInfo> &v, const std::string &userId_to, std::string &msg);
	//解析离线消息
	int ParseOfflineMsg(std::vector<OfflineInfo> &v, const std::string &msg);

	//打包添加好友消息
	int PackAddFriend(const AddFriendInfo &info, std::string &msg);
	//解析添加好友消息
	int ParseAddFriend(AddFriendInfo &info, const std::string &msg);
private:

};

#endif

