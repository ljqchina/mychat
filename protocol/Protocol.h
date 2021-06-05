#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "stringbuffer.h"
#include "writer.h"
#include "document.h"

#include <string>

typedef struct RegisterInfo_
{
    int userid;
    int errcode;
    std::string errMsg;
    std::string nickName;
}RegisterInfo;

class Protocol
{
public:
    Protocol();
    ~Protocol();

public:
    int ParseMsgType(const std::string &msg, int &msgType);
    //int PackRegisterReq(const RegisterInfo &ri, std::string &msg);
    //int UnPackRegisterReq(RegisterInfo &ri, const std::string &msg);

private:

};

#endif

