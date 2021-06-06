#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "stringbuffer.h"
#include "writer.h"
#include "document.h"

#include <string>

typedef struct RegisterInfo_
{
    int sex;
    int age;
    int msgType;
    std::string msgId;
    std::string userId;
    std::string version;
    std::string nickName;
    std::string password;
    int respCode;
    std::string respText;
}RegisterInfo;

class Protocol
{
public:
    Protocol();
    ~Protocol();

public:
    int ParseHeader(const std::string &msg, int &msgType, std::string &version);
    int ParseHeader(const rapidjson::Document &doc, int &msgType, std::string &version);
    int PackRegisterReq(const RegisterInfo &ri, std::string &msg);
    int ParseRegisterReq(RegisterInfo &ri, const std::string &msg);

private:

};

#endif

