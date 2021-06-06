#include "Protocol.h"

Protocol::Protocol()
{
}

Protocol::~Protocol()
{
}

int Protocol::ParseHeader(const std::string &msg, int &msgType, std::string &version)
{
    rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

    if(!doc.HasMember("msgtype") || !doc["msgtype"].IsInt())
        return -1;
    msgType = doc["msgtype"].GetInt();

    if(!doc.HasMember("version"))
        return -1;
    version = doc["version"].GetString();
    return 0;
}

int Protocol::ParseHeader(const rapidjson::Document &doc, int &msgType, std::string &version)
{
    if(!doc.HasMember("msgtype") || !doc["msgtype"].IsInt())
        return -1;
    msgType = doc["msgtype"].GetInt();

    if(!doc.HasMember("version"))
        return -1;
    version = doc["version"].GetString();
    return 0;
}

int Protocol::PackRegisterReq(const RegisterInfo &ri, std::string &msg)
{
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

    w.StartObject();
    
    w.Key("msgtype");
    w.Int(ri.msgType);

    w.Key("version");
    w.String(ri.version.data());

    w.Key("msgid");
    w.String(ri.msgId.data());

    w.Key("userid");
    w.String(ri.userId.data());

    w.Key("nickname");
    w.String(ri.nickName.data());

    w.Key("password");
    w.String(ri.password.data());

    w.Key("sex");
    w.Int(ri.sex);

    w.Key("age");
    w.Int(ri.age);

    w.EndObject();

    msg = buf.GetString();
    return 0;
}

int Protocol::ParseRegisterReq(RegisterInfo &ri, const std::string &msg)
{
    rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

    int msgType;
    std::string version;
    if(ParseHeader(doc, msgType, version) != 0)
    {
        fprintf(stderr, "Parse Header error from json string:\n");
        fprintf(stderr, "%s\n", msg.data());
        return -1;
    }

    return 0; 
}

