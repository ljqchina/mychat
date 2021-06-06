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

class Protocol
{
public:
    Protocol();
    ~Protocol();

public:
    int ParseHeader(const std::string &msg, Header &h);
    int ParseHeader(const rapidjson::Document &doc, Header &h);
    int PackHeader(rapidjson::Writer<rapidjson::StringBuffer> &w, const Header &h);
    int PackHeaderResp(rapidjson::Writer<rapidjson::StringBuffer> &w, const Header &h);
    int PackRegisterReq(const RegisterInfo &ri, std::string &msg);
    int ParseRegisterReq(RegisterInfo &ri, const std::string &msg);

private:

};

#endif

