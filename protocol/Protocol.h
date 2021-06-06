#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "stringbuffer.h"
#include "writer.h"
#include "document.h"

#include <string>

typedef struct Header_
{
    int msgType;
    int respCode;
    std::string msgId;
    std::string version;
    std::string respText;
}Header;

typedef struct RegisterInfo_
{
    Header header;
    int sex;
    int age;
    std::string userId;
    std::string nickName;
    std::string password;
}RegisterInfo;

typedef struct LoginInfo_
{
	Header header;
	int termType;
    std::string userId;
    std::string password;
}LoginInfo;

class Protocol
{
public:
    Protocol();
    ~Protocol();

public:
    int ParseHeader(const std::string &msg, Header &h);
    int ParseHeader(const rapidjson::Document &doc, Header &h);
    int PackHeader(rapidjson::Writer<rapidjson::StringBuffer> &w, const Header &h);
	//响应报文增加响应字段
    int PackHeaderResp(rapidjson::Writer<rapidjson::StringBuffer> &w, const Header &h);
	//打包注册用户报文
    int PackRegister(const RegisterInfo &info, std::string &msg);
	//解析注册用户报文
    int ParseRegister(RegisterInfo &info, const std::string &msg);

	//打包登录报文
    int PackLogin(const LoginInfo &info, std::string &msg);
	//解析登录报文
    int ParseLogin(LoginInfo &info, const std::string &msg);
private:

};

#endif

