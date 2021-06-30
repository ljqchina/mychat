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
	int status;
	std::string userId;
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

typedef struct DelFriendInfo_
{
	Header header;
	std::string userId;
	std::string userId_to;
}DelFriendInfo;

typedef struct ImageInfo_
{
	int flag; //1:data存放的是图片编号ID,2:图片内容的base64编码
	int type; //图片类型,flag为2时使用,1:jpg, 2:png, ...
	std::string data; //
}ImageInfo;

typedef struct ChatInfo_
{
	Header header;
	std::string userId;
	std::string userId_to;
	std::string content;
	std::vector<ImageInfo> imagetVec;//打包时从这里取表情图片信息
}ChatInfo;

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
	int PackOfflineMsg(std::string &msg);
	//int PackOfflineMsg(const std::vector<OfflineInfo> &v, const std::string &userId_to, std::string &msg);
	//解析离线消息
	//int ParseOfflineMsg(std::vector<OfflineInfo> &v, const std::string &msg);

	//打包添加好友消息
	int PackAddFriend(const AddFriendInfo &info, std::string &msg);
	//解析添加好友消息
	int ParseAddFriend(AddFriendInfo &info, const std::string &msg);

	//打包删除好友消息
	int PackDelFriend(const DelFriendInfo &info, std::string &msg);
	//解析删除好友消息
	int ParseDelFriend(DelFriendInfo &info, const std::string &msg);

	//打包聊天消息
	int PackChat(const ChatInfo &info, std::string &msg);
	//解析聊天消息
	int ParseChat(ChatInfo &info, const std::string &msg); private: 
};

#endif

