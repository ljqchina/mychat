#include "Protocol.h"

Protocol::Protocol()
{
}

Protocol::~Protocol()
{
}

int Protocol::ParseMsgType(const std::string &msg, int &msgType)
{
    rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

    if(!doc.HasMember("msgtype"))
        return -1;
    if(!doc["msgtype"].IsInt())
        return -1;
    msgType = doc["msgtype"].GetInt();
    return 0;
}

